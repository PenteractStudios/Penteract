#include "Duke.h"

#include "GameplaySystems.h"
#include "Resources/ResourceMaterial.h"
#include "RangerProjectileScript.h"
#include "PlayerController.h"
#include "BarrelSpawner.h"
#include "AIMovement.h"
#include "GlobalVariables.h"
#include "AttackDronesController.h"
#include "DukeShield.h"
#include "RandomNumberGenerator.h"

#include <string>

#define RNG_SCALE 1.3f
#define RNG_MIN -1.0f
#define RNG_MAX 1.0f



void Duke::Init(UID dukeUID, UID playerUID, UID bulletUID, UID barrelUID, UID chargeColliderUID, UID meleeAttackColliderUID, UID barrelSpawnerUID, UID chargeAttackColliderUID, UID phase2ShieldUID, std::vector<UID> encounterUIDs, AttackDronesController* dronesController, UID punchSlashUID, UID chargeDustUID, UID areaChargeUID, UID chargeTelegraphAreaUID, UID chargePunchVFXUID, UID dustStepLeftUID, UID dustStepRightUID, UID bodyArmorUID)
{
	SetTotalLifePoints(lifePoints);
	characterGameObject = GameplaySystems::GetGameObject(dukeUID);
	player = GameplaySystems::GetGameObject(playerUID);
	chargeCollider = GameplaySystems::GetGameObject(chargeColliderUID);

	meleeAttackCollider = GameplaySystems::GetGameObject(meleeAttackColliderUID);
	chargeAttack = GameplaySystems::GetGameObject(chargeAttackColliderUID);

	GameObject* shieldObj = GameplaySystems::GetGameObject(phase2ShieldUID);
	if (shieldObj) {
		phase2Shield = GET_SCRIPT(shieldObj, DukeShield);
		std::vector<GameObject*> children = shieldObj->GetChildren();
		if (!children.empty()) {
			GameObject* shieldObjChild = children[0];
			if (shieldObjChild) phase2ShieldParticles = shieldObjChild->GetComponent<ComponentParticleSystem>();
		}
	}

	GameObject* barrelSpawnerOBj = GameplaySystems::GetGameObject(barrelSpawnerUID);
	if(barrelSpawnerOBj) barrelSpawneScript = GET_SCRIPT(barrelSpawnerOBj, BarrelSpawner);

	barrel = GameplaySystems::GetResource<ResourcePrefab>(barrelUID);

	GameObject* bulletGO = GameplaySystems::GetGameObject(bulletUID);
	if (bulletGO) {
		bullet = bulletGO->GetComponent<ComponentParticleSystem>();
		if (bullet) {
			bullet->SetParticlesPerSecond(float2(0.0f, 0.0f));
			bullet->SetMaxParticles(attackBurst*2);
			bullet->SetParticlesPerSecond(float2(attackSpeed, attackSpeed));
			bullet->SetDuration((attackBurst + 1) / attackSpeed);
		}
		GameObject* muzzleFlashObj = bulletGO->GetChild("MuzzleFlash");
		if (muzzleFlashObj) {
			ComponentParticleSystem* muzzleFlash = bulletGO->GetChild("MuzzleFlash")->GetComponent<ComponentParticleSystem>();
			if (muzzleFlash) {
				muzzleFlash->SetDuration(attackBurst / attackSpeed);
			}
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

		characterGameObject->GetComponent<ComponentCapsuleCollider>()->Enable();

		//Get audio sources
		int i = 0;

		for (ComponentAudioSource& src : characterGameObject->GetComponents<ComponentAudioSource>()) {
			if (i < static_cast<int>(DUKE_AUDIOS::TOTAL)) dukeAudios[i] = &src;
			i++;
		}
	}
	movementChangeThreshold = moveChangeEvery;
	distanceCorrectionThreshold = distanceCorrectEvery;

	for (auto itr : encounterUIDs) encounters.push_back(GameplaySystems::GetGameObject(itr));

	attackDronesController = dronesController;

	GameObject* punchSlashGO = GameplaySystems::GetGameObject(punchSlashUID);
	if (punchSlashGO) punchSlash = punchSlashGO->GetComponent<ComponentParticleSystem>();

	GameObject* chargeDustGO = GameplaySystems::GetGameObject(chargeDustUID);
	if(chargeDustGO) chargeDust = chargeDustGO->GetComponent<ComponentParticleSystem>();

	if (chargeDust) chargeDustOriginalParticlesPerSecond = chargeDust->GetParticlesPerSecond();

	areaChargeGO = GameplaySystems::GetGameObject(areaChargeUID);
	if (areaChargeGO) {
		GameObject* areaChargeChildGO = areaChargeGO->GetChildren()[0];
		if (areaChargeChildGO) {
			ComponentMeshRenderer* areaChargeMesh = areaChargeChildGO->GetComponent<ComponentMeshRenderer>();
			if (areaChargeMesh) {
				UID areaChargeMaterialUID = areaChargeMesh->GetMaterial();
				areaCharge = GameplaySystems::GetResource<ResourceMaterial>(areaChargeMaterialUID);
			}
		}

	}

	chargeTelegraphAreaGO = GameplaySystems::GetGameObject(chargeTelegraphAreaUID);
	if (chargeTelegraphAreaGO) {
		chargeTelegraphArea = chargeTelegraphAreaGO->GetComponent<ComponentBillboard>();
		dukeScale = dukeTransform->GetGlobalScale().x;
		chargeTelegraphAreaPosOffset = chargeTelegraphAreaGO->GetComponent<ComponentTransform>()->GetPosition().z * dukeScale;
	}

	GameObject* chargePunchVFXGO = GameplaySystems::GetGameObject(chargePunchVFXUID);
	if (chargePunchVFXGO) chargePunchVFX = chargePunchVFXGO->GetComponent<ComponentParticleSystem>();

	bodyArmor = GameplaySystems::GetGameObject(bodyArmorUID);

	GameObject* dustStep = GameplaySystems::GetGameObject(dustStepLeftUID);
	if (dustStep) dustLeftStep = dustStep->GetComponent<ComponentParticleSystem>();

	dustStep = GameplaySystems::GetGameObject(dustStepRightUID);
	if(dustStep) dustRightStep = dustStep->GetComponent<ComponentParticleSystem>();
}

void Duke::ShootAndMove(const float3& playerDirection) {
	// Shoot
	Shoot();
	Move(playerDirection);
}

void Duke::MeleeAttack()
{

	float3 dir = player->GetComponent<ComponentTransform>()->GetGlobalPosition() - dukeTransform->GetGlobalPosition();
	dir.y = 0.0f;
	if (movementScript) movementScript->Orientate(dir);

	if (!hasMeleeAttacked) {
		firstTimePunchParticlesActive = true;
		if (compAnimation) {
			if (compAnimation->GetCurrentState()) {
				compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::PUNCH)]);
				hasMeleeAttacked = true;
			}
		}
	}
}

void Duke::BulletHell() {
	Debug::Log("Bullet hell");
	if (attackDronesController) {
		if (compAnimation) compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::PDA)]);
		ResourceClip* clip = GameplaySystems::GetResource<ResourceClip>(compAnimation->GetCurrentState()->clipUid);
		if (clip) clip->loop = true;
		attackDronesController->StartBulletHell();
	}
}

void Duke::DisableBulletHell() {
	ResourceClip* clip = GameplaySystems::GetResource<ResourceClip>(compAnimation->GetCurrentState()->clipUid);
	if (clip) clip->loop = false;
}

bool Duke::BulletHellActive() const {
	return attackDronesController && attackDronesController->BulletHellActive();
}

bool Duke::BulletHellFinished() const {
	if (!attackDronesController) return true;
	return attackDronesController->BulletHellFinished();
}

bool Duke::PlayerIsInChargeRangeDistance() const{
	float3 playerPosition = player->GetComponent<ComponentTransform>()->GetGlobalPosition();
	return playerPosition.Distance(dukeTransform->GetGlobalPosition()) >= chargeMinimumDistance;
}

bool Duke::IsBulletHellCircular() const
{
	return !BulletHellFinished() && attackDronesController->IsBulletHellCircular();
}

void Duke::InitCharge(DukeState nextState_)
{
	trackingChargeTarget = true;
	state = DukeState::CHARGE;
	this->nextState = nextState_;
	reducedDamaged = true;
	chargeSkidTimer = 0.0f;
	if (compAnimation) {
		compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::CHARGE_START)]);
	}
	if (chargeTelegraphArea) chargeTelegraphArea->Play();
}

void Duke::UpdateCharge(bool forceStop)
{

	if (trackingChargeTarget) {
		float3 dir = player->GetComponent<ComponentTransform>()->GetGlobalPosition() - dukeTransform->GetGlobalPosition();
		dir.y = 0.0f;
		if (movementScript) movementScript->Orientate(dir);
		chargeDir = dir;
		float dist = Distance(player->GetComponent<ComponentTransform>()->GetGlobalPosition(), dukeTransform->GetGlobalPosition());
		if (chargeTelegraphAreaGO) {
			float3 scale = chargeTelegraphAreaGO->GetComponent<ComponentTransform>()->GetScale();
			scale.x = dist / dukeScale;
			chargeTelegraphAreaGO->GetComponent<ComponentTransform>()->SetScale(scale);
			float3 pos = chargeTelegraphAreaGO->GetComponent<ComponentTransform>()->GetPosition();
			pos.z = (scale.x * 0.5f) + chargeTelegraphAreaPosOffset;
			chargeTelegraphAreaGO->GetComponent<ComponentTransform>()->SetPosition(pos);
		}
	}
	if (forceStop || (dukeTransform->GetGlobalPosition() - chargeTarget).Length() <= 0.2f) {
		if (chargeCollider) chargeCollider->Disable();
		if (compAnimation) {
			compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::CHARGE_END)]);
		}
		if (areaChargeGO && areaChargeGO->IsActive()) areaChargeGO->Disable();
		if (chargeDust) chargeDust->SetParticlesPerSecondChild(float2(0.f, 0.f));
		// Perform arm attack (either use the same or another collider as the melee attack)
		if (chargeAttack) chargeAttack->Enable();
		if (chargePunchVFX) chargePunchVFX->PlayChildParticles();
		state = DukeState::CHARGE_ATTACK;

		reducedDamaged = false;
		if (player) {
			PlayerController* playerController = GET_SCRIPT(player, PlayerController);
			if (playerController) playerController->playerOnimaru.shieldBeingUsed = 0.0f;
		}
	}
	else {
		if (areaCharge) {
			float2 matOffset = areaCharge->offset;
			matOffset.y -= (Time::GetDeltaTime()*areaChargeSpeedMultiplier);
			areaCharge->offset = matOffset;
		}
	}
}

void Duke::UpdateChargeAttack() {

	if (chargeSkidTimer < chargeSkidDuration) {
		if (agent) {
			agent->SetMaxSpeed(Lerp(chargeSkidMaxSpeed,chargeSkidMinSpeed, chargeSkidTimer / chargeSkidDuration));

			if (dukeTransform)
				agent->SetMoveTarget(dukeTransform->GetGlobalPosition() + chargeDir, true);
		}

	} else {
		if (agent) {
			agent->SetMaxSpeed(movementSpeed);
			agent->SetMoveTarget(dukeTransform->GetGlobalPosition());
		}
	}

	chargeSkidTimer += Time::GetDeltaTime();

}

void Duke::CallTroops() {
	if (encounters.size() > currentEncounter && encounters[currentEncounter] && !encounters[currentEncounter]->IsActive()) encounters[currentEncounter]->Enable();
	currentEncounter++;
}

void Duke::Move(const float3& playerDirection) {
	movementTimer += Time::GetDeltaTime();
	if (movementTimer >= movementChangeThreshold) {
		perpendicular = playerDirection.Cross(float3(0, 1, 0));
		perpendicular = perpendicular * RandomNumberGenerator::GenerateFloat(-1.0f,1.0f);
		movementChangeThreshold = moveChangeEvery + RandomNumberGenerator::GenerateFloat(RNG_MIN, RNG_MAX);
		movementTimer = 0.f;
	}
	distanceCorrectionTimer += Time::GetDeltaTime();
	if (distanceCorrectionTimer >= distanceCorrectionThreshold) {
		perpendicular += playerDirection.Normalized() * (playerDirection.Length() - searchRadius);
		distanceCorrectionThreshold = distanceCorrectEvery + RandomNumberGenerator::GenerateFloat(RNG_MIN, RNG_MAX);
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
		ComponentTransform* playerTransform = player->GetComponent<ComponentTransform>();
		PlayerController* playerController = GET_SCRIPT(player, PlayerController);
		float3 targetDirection = playerTransform->GetGlobalPosition() + playerTransform->GetFront() + float3(0.f, 3.f, 0.f) - bulletTransform->GetGlobalPosition();
		if (playerController) {
			float3 dir = playerController->playerFang.IsActive() ? playerController->playerFang.GetDirection() : playerController->playerOnimaru.GetDirection();
			targetDirection += dir * 2.f;
		}
		// Limit the emitter rotation (Just rotate when angle is minor than 30 degrees)
		float dotProd = targetDirection.Normalized().Dot(dukeTransform->GetFront().Normalized());
		if (dotProd > 0.856) {
			bulletTransform->SetGlobalRotation(Quat::LookAt(float3(0, 1, 0), targetDirection, float3(0, 0, -1), float3(0, 1, 0)));
		}
	}
	if (isShooting) {
		isShootingTimer += Time::GetDeltaTime();
		if (isShootingTimer >= attackBurst / attackSpeed) {
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
		attackTimePool = (attackBurst + 1) / attackSpeed + timeInterBurst + RandomNumberGenerator::GenerateFloat(0.4f, 1.5f);
		isShooting = true;
		isShootingTimer = 0.f;
		// Animation
		if (state != DukeState::SHOOT_SHIELD) {
			if (compAnimation) {
				if (compAnimation->GetCurrentState()) {
					compAnimation->SendTriggerSecondary(compAnimation->GetCurrentState()->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::SHOOT)]);
				}
			}
		}
	}
}

void Duke::ThrowBarrels() {
	if (compAnimation->GetCurrentState()->name != animationStates[static_cast<int>(DUKE_ANIMATION_STATES::PDA)]) {
		compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::PDA)]);
		instantiateBarrel = true;
	}
}

//Not to be confused with AIDuke StartUsing shield, this one manages both state and animations
void Duke::StartUsingShield() {

	if (isShooting) {
		StopShooting();
	}

	state = DukeState::SHOOT_SHIELD;
	if (compAnimation) {
		if (compAnimation->GetCurrentState()) {
			compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::SHOOT_SHIELD)]);
		}
	}

}

void Duke::BePushed() {
	state = DukeState::PUSHED;
	beingPushed = true;

	if (compAnimation) {
		if (compAnimation->GetCurrentStateSecondary()) {
			compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + compAnimation->GetCurrentState()->name);
		}
		if (compAnimation->GetCurrentState()) {
			compAnimation->SendTriggerSecondary(compAnimation->GetCurrentState()->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::PUSHED)]);
		}
	}

	if (agent) {
		agent->SetMaxSpeed(movementSpeed);
	}

}

void Duke::BecomeStunned() {
	if (compAnimation) {
		if (compAnimation) {
			if (compAnimation->GetCurrentStateSecondary()) {
				compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + compAnimation->GetCurrentState()->name);
			}
			if (compAnimation->GetCurrentState()) {
				compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::STUN)]);
			}
		}
	}
	if (agent) {
		agent->SetMaxSpeed(movementSpeed);
	}
}

void Duke::TeleportDuke(bool toMapCenter)
{
	if (toMapCenter) {
		if (agent) {
			agent->SetMoveTarget(phase2CenterPosition);
			agent->SetMaxSpeed(movementSpeed * 2.f);
		}
		float3 dir = phase2CenterPosition - dukeTransform->GetGlobalPosition();
		dir.y = 0;
		movementScript->Orientate(dir);
		if (compAnimation) {
			compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + animationStates[Duke::DUKE_ANIMATION_STATES::WALK_NO_AIM]);
		}
		isInArena = false;
	}
	else {
		if (phase2Shield) phase2Shield->FadeShield();
		if (phase2ShieldParticles) phase2ShieldParticles->StopChildParticles();
		mustAddAgent = true;
		isInArena = true;
	}
}

void Duke::OnAnimationFinished()
{
	if (!compAnimation) return;
	State* localCurrentState = compAnimation->GetCurrentState();
	if (!localCurrentState) return;

	if (localCurrentState->name == animationStates[static_cast<int>(DUKE_ANIMATION_STATES::PUNCH)]) {
		hasMeleeAttacked = false;
		compAnimation->SendTrigger(localCurrentState->name + animationStates[DUKE_ANIMATION_STATES::IDLE]);
		state = DukeState::BASIC_BEHAVIOUR;
	} else if (localCurrentState->name == animationStates[static_cast<int>(DUKE_ANIMATION_STATES::PDA)]) {
		compAnimation->SendTrigger(animationStates[static_cast<int>(DUKE_ANIMATION_STATES::PDA)] + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::IDLE)]);
	} else if (localCurrentState->name == animationStates[static_cast<int>(DUKE_ANIMATION_STATES::CHARGE_START)]) {
		agent->SetMoveTarget(chargeTarget);
		agent->SetMaxSpeed(chargeSpeed);
		if (chargeCollider) chargeCollider->Enable();
		compAnimation->SendTrigger(localCurrentState->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::CHARGE)]);
		if (areaChargeGO && !areaChargeGO->IsActive()) {
			areaCharge->offset = float2(0, 0);
			areaChargeGO->Enable();
		}
		if (chargeDust) {
			chargeDust->SetParticlesPerSecondChild(chargeDustOriginalParticlesPerSecond);
			chargeDust->PlayChildParticles();
		}
	} else if (localCurrentState->name == animationStates[static_cast<int>(DUKE_ANIMATION_STATES::CHARGE_END)]) {
		if (chargeAttack) chargeAttack->Disable();
		state = nextState;
		agent->SetMaxSpeed(movementSpeed);
		compAnimation->SendTrigger(localCurrentState->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::IDLE)]);
	} else if (localCurrentState->name == animationStates[static_cast<int>(DUKE_ANIMATION_STATES::DEATH)]) {
		isDead = true;
	} else if (localCurrentState->name == animationStates[static_cast<int>(DUKE_ANIMATION_STATES::ENRAGE)]) {
		state = DukeState::BASIC_BEHAVIOUR;
		if (bodyArmor && !bodyArmor->IsActive()) bodyArmor->Enable();
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
				if (punchSlash && firstTimePunchParticlesActive) {
					punchSlash->PlayChildParticles();
					firstTimePunchParticlesActive = false;
				}
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
		} else if (strcmp(eventName, "ThrowBarrels") == 0 && instantiateBarrel) {
			if (startSpawnBarrel && barrelSpawneScript) {
				barrelSpawneScript->SpawnBarrels();
				startSpawnBarrel = false;
			}
			else {
				InstantiateBarrel();
				startSpawnBarrel = true;
			}
			instantiateBarrel = false;
		} else if (strcmp(eventName, "FootstepLeft") == 0) {
			if (dustLeftStep) dustLeftStep->PlayChildParticles();
		} else if (strcmp(eventName, "FootstepRight") == 0) {
			if (dustRightStep) dustRightStep->PlayChildParticles();
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

void Duke::StartPhase2Shield()
{
	float3 dir = phase2CenterPosition - dukeTransform->GetGlobalPosition();
	dir.y = 0;
	movementScript->Orientate(dir);

	if (compAnimation && compAnimation->GetCurrentState()->name != animationStates[Duke::DUKE_ANIMATION_STATES::IDLE] &&
		compAnimation->GetCurrentState()->name != animationStates[Duke::DUKE_ANIMATION_STATES::PDA] &&
		(dukeTransform->GetGlobalPosition() - phase2CenterPosition).Length() <= 0.5f) {

		if (phase2Shield) phase2Shield->InitShield();
		if (phase2ShieldParticles) phase2ShieldParticles->PlayChildParticles();
		compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + animationStates[Duke::DUKE_ANIMATION_STATES::PDA]);
		CallTroops();
	}
}

void Duke::InstantiateBarrel()
{
	//Instantiate barrel and play animation throw barrels for Duke and the barrel
	if (barrel) {
		GameplaySystems::Instantiate(barrel, player->GetComponent<ComponentTransform>()->GetGlobalPosition(), Quat(0, 0, 0, 1));
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

void Duke::ActivateDissolve(UID dissolveMaterialID) {
	ComponentMeshRenderer* meshRenderer = meshObj->GetComponent<ComponentMeshRenderer>();
	if (meshRenderer && dissolveMaterialID != 0) {
		meshRenderer->SetMaterial(dissolveMaterialID);
		meshRenderer->PlayDissolveAnimation();
	}
}

void Duke::SetCriticalMode(bool activate)
{
	criticalMode = activate;
	if (activate) {
		if (compAnimation) {
			StopShooting();
			compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + animationStates[Duke::DUKE_ANIMATION_STATES::ENRAGE]);
		}
		state = DukeState::INVULNERABLE;

	}
	else {
		if (bodyArmor && bodyArmor->IsActive()) bodyArmor->Disable();
		state = DukeState::SHOOT_SHIELD;
		CallTroops();
		StartUsingShield();
	}
}

ComponentMeshRenderer* Duke::GetDukeMeshRenderer() const {
	return meshObj->GetComponent<ComponentMeshRenderer>();
}
