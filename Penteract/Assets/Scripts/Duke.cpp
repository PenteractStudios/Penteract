#include "Duke.h"

#include "GameplaySystems.h"
#include "RangerProjectileScript.h"
#include "PlayerController.h"

#include <string>

#define RNG_SCALE 1.3f

std::uniform_real_distribution<> rng(-1.0f, 1.0f);

void Duke::Init(UID dukeUID, UID playerUID, UID bulletUID, UID barrelUID, UID chargeColliderUID)
{
	gen = std::minstd_rand(rd());

	SetTotalLifePoints(lifePoints);
	characterGameObject = GameplaySystems::GetGameObject(dukeUID);
	player = GameplaySystems::GetGameObject(playerUID);
	chargeCollider = GameplaySystems::GetGameObject(chargeColliderUID);

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
	this->nextState = nextState;
	reducedDamaged = true;
	if (chargeCollider) chargeCollider->Enable();
	Debug::Log("Electric Tackle!");
}

void Duke::UpdateCharge(bool forceStop)
{
	if (forceStop || (dukeTransform->GetGlobalPosition() - chargeTarget).Length() <= 0.2f) {
		if (chargeCollider) chargeCollider->Disable();
		EndCharge();
	}
}

void Duke::EndCharge()
{
	// Perform arm attack (either use the same or another collider as the melee attack)
	state = nextState;
	reducedDamaged = false;
	if (player) {
		PlayerController* playerController = GET_SCRIPT(player, PlayerController);
		if (playerController) playerController->playerOnimaru.shieldBeingUsed = 0.0f;
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

	if (compAnimation->GetCurrentState()->name != animationStates[static_cast<int>(DUKE_ANIMATION_STATES::PDA)]) {
		compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::PDA)]);
	}
}

void Duke::OnAnimationFinished()
{
	if (compAnimation->GetCurrentState()->name == animationStates[static_cast<int>(DUKE_ANIMATION_STATES::PDA)]) {
		compAnimation->SendTrigger(animationStates[static_cast<int>(DUKE_ANIMATION_STATES::PDA)] + animationStates[static_cast<int>(DUKE_ANIMATION_STATES::IDLE)]);		
	}
}

void Duke::OnAnimationSecondaryFinished()
{
}

void Duke::OnAnimationEvent(StateMachineEnum stateMachineEnum, const char* eventName)
{
	switch (stateMachineEnum) {
	case StateMachineEnum::PRINCIPAL:
		if (std::strcmp(eventName, "ThrowBarrels") == 0) {
			InstantiateBarrel();
		}
		break;
	case StateMachineEnum::SECONDARY:
		break;
	}
}

void Duke::InstantiateBarrel()
{
	//Instantiate barrel and play animation throw barrels for Duke and the barrel
	if (barrel) {
		GameObject* auxBarrel = GameplaySystems::Instantiate(barrel, player->GetComponent<ComponentTransform>()->GetGlobalPosition(), Quat(0, 0, 0, 1));
	}
}
