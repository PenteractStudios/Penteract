#include "Duke.h"

#include "GameplaySystems.h"
#include "RangerProjectileScript.h"
#include "PlayerController.h"

#include <string>

#define RNG_SCALE 1.3f

std::uniform_real_distribution<> rng(-1.0f, 1.0f);

void Duke::Init(UID dukeUID, UID playerUID, UID bulletUID, UID barrelUID, UID chargeColliderUID, std::vector<UID> encounterUIDs) {
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

	for (auto itr : encounterUIDs) encounters.push_back(GameplaySystems::GetGameObject(itr));
}

void Duke::ShootAndMove(const float3& playerDirection) {
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

void Duke::MeleeAttack() {
	Debug::Log("Hooryah!");
}

void Duke::BulletHell() {
	Debug::Log("Bullet hell");
}

void Duke::InitCharge(DukeState nextState) {
	this->nextState = nextState;
	reducedDamaged = true;
	if (chargeCollider) chargeCollider->Enable();
	Debug::Log("Electric Tackle!");
}

void Duke::UpdateCharge(bool forceStop) {
	if (forceStop || (dukeTransform->GetGlobalPosition() - chargeTarget).Length() <= 0.2f) {
		if (chargeCollider) chargeCollider->Disable();
		EndCharge();
	}
}

void Duke::EndCharge() {
	// Perform arm attack (either use the same or another collider as the melee attack)
	state = nextState;
	reducedDamaged = false;
	if (player) {
		PlayerController* playerController = GET_SCRIPT(player, PlayerController);
		if (playerController) playerController->playerOnimaru.shieldBeingUsed = 0.0f;
	}
}

void Duke::CallTroops() {
	Debug::Log("Come, guys!");
	if (encounters.size() > currentEncounter && encounters[currentEncounter] && !encounters[currentEncounter]->IsActive()) encounters[currentEncounter]->Enable();
	currentEncounter++;
}

void Duke::Shoot() {
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

void Duke::ThrowBarrels() {
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

void Duke::OnAnimationFinished() {
}

void Duke::OnAnimationSecondaryFinished() {
}

void Duke::OnAnimationEvent(StateMachineEnum stateMachineEnum, const char* eventName) {
}
