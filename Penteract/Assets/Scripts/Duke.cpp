#include "Duke.h"

#include "GameplaySystems.h"
#include "RangerProjectileScript.h"
#include "PlayerController.h"
#include "AIMovement.h"

#include <string>

#define RNG_SCALE 1.3f

std::uniform_real_distribution<> rng(-1.0f, 1.0f);

void Duke::Init(UID dukeUID, UID playerUID, UID bulletUID, UID barrelUID, UID chargeColliderUID, UID chargeAttackColliderUID)
{
	gen = std::minstd_rand(rd());

	SetTotalLifePoints(lifePoints);
	characterGameObject = GameplaySystems::GetGameObject(dukeUID);
	player = GameplaySystems::GetGameObject(playerUID);
	chargeCollider = GameplaySystems::GetGameObject(chargeColliderUID);
	chargeAttack = GameplaySystems::GetGameObject(chargeAttackColliderUID);

	barrel = GameplaySystems::GetResource<ResourcePrefab>(barrelUID);

	GameObject* bulletGO = GameplaySystems::GetGameObject(bulletUID);
	if (bulletGO) {
		bullet = bulletGO->GetComponent<ComponentParticleSystem>();
		if (bullet) {
			bullet->SetParticlesPerSecond(float2(0.0f, 0.0f));
			bullet->Play();
			bullet->SetMaxParticles(attackBurst);
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
}

void Duke::ShootAndMove(const float3& playerDirection)
{
	// Shoot
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
	agent->SetMoveTarget(dukeTransform->GetGlobalPosition() + perpendicular);
	Shoot();
	Debug::Log("I'm moving while shooting");
}

void Duke::MeleeAttack()
{
	Debug::Log("Hooryah!");
}

void Duke::BulletHell()
{
	Debug::Log("Bullet hell");
}

void Duke::InitCharge(DukeState nextState)
{
	trackingChargeTarget = true;
	state = DukeState::CHARGE;
	this->nextState = nextState;
	reducedDamaged = true;

	if (compAnimation) {
		compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::CHARGE_START)]);
	}
	Debug::Log("Electric Tackle!");
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

void Duke::CallTroops()
{
	Debug::Log("Come, guys!");
}

void Duke::Shoot()
{
	attackTimePool -= Time::GetDeltaTime();
	if (attackTimePool <= 0) {
		if (bullet) {
			if (!meshObj) return;
			bullet->PlayChildParticles();
		}
		attackTimePool = (attackBurst / attackSpeed) + timeInterBurst + rng(gen) * RNG_SCALE;
		// Animation
	}
	Debug::Log("PIUM!");
}

void Duke::ThrowBarrels()
{
	Debug::Log("Here, barrel in your face!");

	float height = 15.0f;
	float3 playerPos = player->GetComponent<ComponentTransform>()->GetGlobalPosition();

	//Instantiate barrel and play animation throw barrels for Duke and the barrel
	if (barrel) {
		GameObject* auxBarrel = GameplaySystems::Instantiate(barrel, playerPos + float3(0.0f, height, 0.0f), Quat(0, 0, 0, 1));
	}
	/*if (auxBarrel->GetComponent<ComponentParticleSystem>()) {
		auxBarrel->GetComponent<ComponentParticleSystem>()->Play();
	}*/

	//When animation finished, set player + random offset position and the barrel falls to this position
}

void Duke::OnAnimationFinished()
{
	if (compAnimation && compAnimation->GetCurrentState()) {
		std::string currentStateName = compAnimation->GetCurrentState()->name;
		if (currentStateName == animationStates[static_cast<int>(DUKE_ANIMATION_STATES::CHARGE_START)]) {
			agent->SetMoveTarget(chargeTarget);
			agent->SetMaxSpeed(chargeSpeed);
			if (chargeCollider) chargeCollider->Enable();
			compAnimation->SendTrigger(currentStateName + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::CHARGE)]);
		} else if (currentStateName == animationStates[static_cast<int>(DUKE_ANIMATION_STATES::CHARGE_END)]) {
			if (chargeAttack) chargeAttack->Disable();
			state = nextState;
			agent->SetMaxSpeed(movementSpeed);
			compAnimation->SendTrigger(currentStateName + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::IDLE)]);
		}
	}
}

void Duke::OnAnimationSecondaryFinished()
{
}

void Duke::OnAnimationEvent(StateMachineEnum stateMachineEnum, const char* eventName)
{
	switch (stateMachineEnum) {
		case StateMachineEnum::PRINCIPAL:
			if (strcmp(eventName, "StopTracking") == 0) {
				if (!trackingChargeTarget) return;
				trackingChargeTarget = false;
				chargeTarget = player->GetComponent<ComponentTransform>()->GetGlobalPosition();
			}
			break;
		default:
			break;
	}
}
