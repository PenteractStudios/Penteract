#include "Duke.h"

#include "GameplaySystems.h"
#include "RangerProjectileScript.h"
#include "PlayerController.h"
#include "BarrelSpawner.h"
#include "AIMovement.h"
#include "AttackDronesController.h"

#include <string>

#define RNG_SCALE 1.3f

std::uniform_real_distribution<float> rng(-1.0f, 1.0f);

void Duke::Init(UID dukeUID, UID playerUID, UID bulletUID, UID barrelUID, UID chargeColliderUID, UID meleeAttackColliderUID, UID barrelSpawnerUID, UID chargeAttackColliderUID, std::vector<UID> encounterUIDs, AttackDronesController* dronesController)
{
	gen = std::minstd_rand(rd());

	SetTotalLifePoints(lifePoints);
	characterGameObject = GameplaySystems::GetGameObject(dukeUID);
	player = GameplaySystems::GetGameObject(playerUID);
	chargeCollider = GameplaySystems::GetGameObject(chargeColliderUID);

	meleeAttackCollider = GameplaySystems::GetGameObject(meleeAttackColliderUID);
	chargeAttack = GameplaySystems::GetGameObject(chargeAttackColliderUID);

	barrelSpawneScript = GET_SCRIPT(GameplaySystems::GetGameObject(barrelSpawnerUID), BarrelSpawner);

	barrel = GameplaySystems::GetResource<ResourcePrefab>(barrelUID);

	GameObject* bulletGO = GameplaySystems::GetGameObject(bulletUID);
	if (bulletGO) {
		bullet = bulletGO->GetComponent<ComponentParticleSystem>();
		if (bullet) {
			bullet->SetParticlesPerSecond(float2(0.0f, 0.0f));
			bullet->Play();
			bullet->SetMaxParticles(attackBurst*2);
			bullet->SetParticlesPerSecond(float2(attackSpeed, attackSpeed));
			bullet->SetDuration(attackBurst / attackSpeed);
		}
	}

	if (characterGameObject) {
		meshObj = characterGameObject->GetChildren()[0];
		dukeTransform = characterGameObject->GetComponent<ComponentTransform>();
		agent = characterGameObject->GetComponent<ComponentAgent>();
		compAnimation = characterGameObject->GetComponent<ComponentAnimation>();
		movementScript = GET_SCRIPT(characterGameObject, AIMovement);

		if (compAnimation) {
			currentState = compAnimation->GetCurrentState();
		}

		if (agent) {
			agent->AddAgentToCrowd();
			agent->SetMaxSpeed(movementSpeed);
			agent->SetMaxAcceleration(MAX_ACCELERATION);
		}

		if (characterGameObject) {
			characterGameObject->GetComponent<ComponentCapsuleCollider>()->Enable();

			//Get audio sources
			int i = 0;

			for (ComponentAudioSource& src : characterGameObject->GetComponents<ComponentAudioSource>()) {
				if (i < static_cast<int>(DUKE_AUDIOS::TOTAL)) dukeAudios[i] = &src;
				i++;
			}
		}
	}
	movementChangeThreshold = moveChangeEvery;
	distanceCorrectionThreshold = distanceCorrectEvery;

	for (auto itr : encounterUIDs) encounters.push_back(GameplaySystems::GetGameObject(itr));

	attackDronesController = dronesController;
}

void Duke::ShootAndMove(const float3& playerDirection) {
	// Shoot
	Shoot();
	Move(playerDirection);
}

void Duke::MeleeAttack()
{
	if (!hasMeleeAttacked) {
		if (compAnimation) {
			if (compAnimation->GetCurrentState()) {
				compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + animationStates[DUKE_ANIMATION_STATES::PUNCH]);
				hasMeleeAttacked = true;
			}
		}
	}
}

void Duke::BulletHell() {
	Debug::Log("Bullet hell");
	if (attackDronesController) {
		compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::PDA)]);
		ResourceClip* clip = GameplaySystems::GetResource<ResourceClip>(compAnimation->GetCurrentState()->clipUid);
		if (clip) clip->loop = true;
		attackDronesController->StartBulletHell();
	}
}

void Duke::DisableBulletHell() {
	ResourceClip* clip = GameplaySystems::GetResource<ResourceClip>(compAnimation->GetCurrentState()->clipUid);
	if (clip) clip->loop = false;
}

bool Duke::BulletHellActive() {
	return attackDronesController && attackDronesController->BulletHellActive();
}

bool Duke::BulletHellFinished() {
	if (!attackDronesController) return true;
	return attackDronesController->BulletHellFinished();
}

void Duke::InitCharge(DukeState nextState_)
{
	trackingChargeTarget = true;
	state = DukeState::CHARGE;
	this->nextState = nextState_;
	reducedDamaged = true;

	if (compAnimation) {
		compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::CHARGE_START)]);
	}
}

void Duke::UpdateCharge(bool forceStop)
{

	if (trackingChargeTarget) {
		float3 dir = player->GetComponent<ComponentTransform>()->GetGlobalPosition() - dukeTransform->GetGlobalPosition();
		dir.y = 0.0f;
		movementScript->Orientate(dir);
	}
	if (forceStop || (dukeTransform->GetGlobalPosition() - chargeTarget).Length() <= 0.2f) {
		if (chargeCollider) chargeCollider->Disable();
		if (compAnimation) {
			compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::CHARGE_END)]);
		}
		// Perform arm attack (either use the same or another collider as the melee attack)
		if (chargeAttack) chargeAttack->Enable();
		state = DukeState::CHARGE_ATTACK;
		reducedDamaged = false;
		if (player) {
			PlayerController* playerController = GET_SCRIPT(player, PlayerController);
			if (playerController) playerController->playerOnimaru.shieldBeingUsed = 0.0f;
		}
	}
}

void Duke::CallTroops() {
	if (encounters.size() > currentEncounter && encounters[currentEncounter] && !encounters[currentEncounter]->IsActive()) encounters[currentEncounter]->Enable();
	currentEncounter++;
}

void Duke::Move(const float3& playerDirection) {
	movementTimer += Time::GetDeltaTime();
	if (movementTimer >= movementChangeThreshold) {
		perpendicular = playerDirection.Cross(float3(0, 1, 0));
		perpendicular = perpendicular * rng(gen);
		movementChangeThreshold = moveChangeEvery + rng(gen);
		movementTimer = 0.f;
	}
	distanceCorrectionTimer += Time::GetDeltaTime();
	if (distanceCorrectionTimer >= distanceCorrectionThreshold) {
		perpendicular += playerDirection.Normalized() * (playerDirection.Length() - searchRadius);
		distanceCorrectionThreshold = distanceCorrectEvery + rng(gen);
		distanceCorrectionTimer = 0.f;
	}

	Navigation::Raycast(dukeTransform->GetGlobalPosition(), dukeTransform->GetGlobalPosition() + perpendicular, navigationHit, navigationHitPos);
	if (navigationHit) perpendicular = -perpendicular;
	if (agent) agent->SetMoveTarget(navigationHitPos);
	int movementAnim = GetWalkAnimation();
	if (compAnimation && compAnimation->GetCurrentState()->name != animationStates[movementAnim]) {
		compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + animationStates[movementAnim]);
	}
}

void Duke::Shoot()
{
	attackTimePool -= Time::GetDeltaTime();
	if (bullet) {
		ComponentTransform* bulletTransform = bullet->GetOwner().GetComponent<ComponentTransform>();
		float3 targetDirection = player->GetComponent<ComponentTransform>()->GetGlobalPosition() + float3(0.f, 2.7f, 0.f) - bulletTransform->GetGlobalPosition();
		bulletTransform->SetGlobalRotation(Quat::LookAt(float3(0,1,0), targetDirection, float3(0,0,-1), float3(0,1,0)));
	}
	if (isShooting) {
		isShootingTimer += Time::GetDeltaTime();
		if (isShootingTimer >= (attackBurst-1) / attackSpeed) {
			isShooting = false;
			if (compAnimation && compAnimation->GetCurrentStateSecondary()) {
				if (compAnimation->GetCurrentStateSecondary()->name == animationStates[static_cast<int>(DUKE_ANIMATION_STATES::SHOOT)]) {
					compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + compAnimation->GetCurrentState()->name);
				}
			}
		}
	} else if (attackTimePool <= 0) {
		if (bullet) {
			if (!meshObj) return;
			bullet->PlayChildParticles();
		}
		attackTimePool = (attackBurst / attackSpeed) + timeInterBurst + rng(gen) * RNG_SCALE;
		isShooting = true;
		isShootingTimer = 0.f;
		// Animation
		if (compAnimation) {
			compAnimation->SendTriggerSecondary(compAnimation->GetCurrentState()->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::SHOOT)]);
		}
	}
}

void Duke::ThrowBarrels() {
	if (compAnimation->GetCurrentState()->name != animationStates[static_cast<int>(DUKE_ANIMATION_STATES::PDA)]) {
		compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::PDA)]);
		instantiateBarrel = true;
	}
}

void Duke::StartUsingShield() {
	state = DukeState::SHOOT_SHIELD;
	if (compAnimation) {
		if (compAnimation->GetCurrentStateSecondary()) {
			compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::SHOOT_SHIELD)]);
		} else if (compAnimation->GetCurrentState()) {
			compAnimation->SendTriggerSecondary(compAnimation->GetCurrentState()->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::SHOOT_SHIELD)]);
		}
		compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::IDLE)]);
	}
}

void Duke::StopUsingShield() {
	if (compAnimation) {
		if (compAnimation->GetCurrentState() && compAnimation->GetCurrentStateSecondary()) {
			compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + compAnimation->GetCurrentState()->name);
		}
	}
}

void Duke::OnAnimationFinished()
{
	if (!compAnimation) return;
	State* currentState = compAnimation->GetCurrentState();
	if (!currentState) return;

	if (currentState->name == animationStates[static_cast<int>(DUKE_ANIMATION_STATES::PUNCH)]) {
		hasMeleeAttacked = false;
		compAnimation->SendTrigger(currentState->name + animationStates[DUKE_ANIMATION_STATES::IDLE]);
		state = DukeState::BASIC_BEHAVIOUR;
	} else if (currentState->name == animationStates[static_cast<int>(DUKE_ANIMATION_STATES::PDA)]) {
		compAnimation->SendTrigger(animationStates[static_cast<int>(DUKE_ANIMATION_STATES::PDA)] + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::IDLE)]);
	} else if (currentState->name == animationStates[static_cast<int>(DUKE_ANIMATION_STATES::CHARGE_START)]) {
		agent->SetMoveTarget(chargeTarget);
		agent->SetMaxSpeed(chargeSpeed);
		if (chargeCollider) chargeCollider->Enable();
		compAnimation->SendTrigger(currentState->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::CHARGE)]);
	} else if (currentState->name == animationStates[static_cast<int>(DUKE_ANIMATION_STATES::CHARGE_END)]) {
		if (chargeAttack) chargeAttack->Disable();
		state = nextState;
		agent->SetMaxSpeed(movementSpeed);
		compAnimation->SendTrigger(currentState->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::IDLE)]);
	} else if (currentState->name == animationStates[static_cast<int>(DUKE_ANIMATION_STATES::DEATH)]) {
		GameplaySystems::DestroyGameObject(characterGameObject);
		SceneManager::ChangeScene(winSceneUID); // TODO: Replace with the correct trigger (for the video or whatever)
	} else if (currentState->name == animationStates[static_cast<int>(DUKE_ANIMATION_STATES::ENRAGE)]) {
		state = DukeState::BASIC_BEHAVIOUR;
	} else if (currentState->name == animationStates[static_cast<int>(DUKE_ANIMATION_STATES::STUN)] && state == DukeState::INVULNERABLE) {
		CallTroops();
		StartUsingShield();
	}
}

void Duke::OnAnimationSecondaryFinished() {
}

void Duke::OnAnimationEvent(StateMachineEnum stateMachineEnum, const char* eventName)
{
	switch (stateMachineEnum)
	{
	case StateMachineEnum::PRINCIPAL:
		if (strcmp(eventName, "EnablePunch") == 0) {
			if (meleeAttackCollider && !meleeAttackCollider->IsActive()) {
				meleeAttackCollider->Enable();
			}
		} else if (strcmp(eventName, "DisablePunch") == 0) {
			if (meleeAttackCollider && meleeAttackCollider->IsActive()) {
				meleeAttackCollider->Disable();
			}
		} else if (strcmp(eventName, "StopTracking") == 0) {
			if (!trackingChargeTarget) return;
			trackingChargeTarget = false;
			float3 dukePos = dukeTransform->GetGlobalPosition();
			if ((player->GetComponent<ComponentTransform>()->GetGlobalPosition() - dukePos).Length() <= chargeMinimumDistance) {
				bool result;
				Navigation::Raycast(dukePos, dukePos + chargeMinimumDistance * dukeTransform->GetFront(), result, chargeTarget);
			}
			else {
				chargeTarget = player->GetComponent<ComponentTransform>()->GetGlobalPosition();
			}
		}

		if (strcmp(eventName, "ThrowBarrels") == 0 && instantiateBarrel) {
			if (startSpawnBarrel) {
				barrelSpawneScript->SpawnBarrels();
				startSpawnBarrel = false;
			}
			else {
				InstantiateBarrel();
				startSpawnBarrel = true;
			}
			instantiateBarrel = false;
		}
		break;
	case StateMachineEnum::SECONDARY:
		break;
	default:
		break;
	}
}

void Duke::StopShooting()
{
	if (compAnimation && compAnimation->GetCurrentStateSecondary()) {
		if (compAnimation->GetCurrentStateSecondary()->name == animationStates[Duke::DUKE_ANIMATION_STATES::SHOOT]) {
			compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name +
				compAnimation->GetCurrentState()->name);
		}
	}
	if (isShooting) isShooting = false;
}

void Duke::InstantiateBarrel()
{
	//Instantiate barrel and play animation throw barrels for Duke and the barrel
	if (barrel) {
		GameObject* auxBarrel = GameplaySystems::Instantiate(barrel, player->GetComponent<ComponentTransform>()->GetGlobalPosition(), Quat(0, 0, 0, 1));
	}
}

int Duke::GetWalkAnimation()
{
	float dot = Dot(perpendicular.Normalized(), dukeTransform->GetFront());
	float3 cross = Cross(perpendicular.Normalized(), dukeTransform->GetFront());

	int animNum = 0;
	if (dot >= 0.707) {
		animNum = static_cast<int>(DUKE_ANIMATION_STATES::WALK_FORWARD);
	} else if (dot <= -0.707) {
		animNum = static_cast<int>(DUKE_ANIMATION_STATES::WALK_BACK);
	} else {
		if (cross.y > 0) {
			animNum = static_cast<int>(DUKE_ANIMATION_STATES::WALK_RIGHT);
		}
		else {
			animNum = static_cast<int>(DUKE_ANIMATION_STATES::WALK_LEFT);
		}
	}
	return animNum;
}
