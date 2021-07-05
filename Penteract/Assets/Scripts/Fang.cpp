#include "Fang.h"
#include "GameplaySystems.h"
#include "HUDController.h"
#include "UltimateFang.h"
#include "CameraController.h"

void Fang::Init(UID fangUID, UID trailUID, UID leftGunUID, UID rightGunUID, UID bulletUID, UID cameraUID, UID canvasUID, UID fangUltimateUID)
{
	SetTotalLifePoints(lifePoints);
	characterGameObject = GameplaySystems::GetGameObject(fangUID);

	if (characterGameObject && characterGameObject->GetParent()) {
		playerMainTransform = characterGameObject->GetParent()->GetComponent<ComponentTransform>();
		agent = characterGameObject->GetParent()->GetComponent<ComponentAgent>();
		compAnimation = characterGameObject->GetComponent<ComponentAnimation>();
		GameObject* cameraAux = GameplaySystems::GetGameObject(cameraUID);
		if (cameraAux) {
			lookAtMouseCameraComp = cameraAux->GetComponent<ComponentCamera>();
			cameraController = GET_SCRIPT(cameraAux, CameraController);
		}
		//right gun
		GameObject* gunAux = GameplaySystems::GetGameObject(rightGunUID);
		if (gunAux) rightGunTransform = gunAux->GetComponent<ComponentTransform>();
		//left gun
		gunAux = GameplaySystems::GetGameObject(leftGunUID);
		if (gunAux) leftGunTransform = gunAux->GetComponent<ComponentTransform>();
		lookAtMousePlanePosition = leftGunTransform->GetGlobalPosition();
		trail = GameplaySystems::GetResource<ResourcePrefab>(trailUID);
		bullet = GameplaySystems::GetResource<ResourcePrefab>(bulletUID);

		if (compAnimation) {
			currentState = compAnimation->GetCurrentState();
		}

		if (agent) {
			agent->SetMaxSpeed(movementSpeed);
			agent->SetMaxAcceleration(MAX_ACCELERATION);
		}
	}
	GameObject* canvasGO = GameplaySystems::GetGameObject(canvasUID);
	if (canvasGO) {
		hudControllerScript = GET_SCRIPT(canvasGO, HUDController);
	}
	if (characterGameObject) {

		characterGameObject->GetComponent<ComponentCapsuleCollider>()->Enable();

		//Get audio sources
		int i = 0;

		for (ComponentAudioSource& src : characterGameObject->GetComponents<ComponentAudioSource>()) {
			if (i < static_cast<int>(AudioPlayer::TOTAL)) playerAudios[i] = &src;
			i++;
		}

	}

	GameObject* fangUltimateGameObject = GameplaySystems::GetGameObject(fangUltimateUID);
	if (fangUltimateGameObject) {
		ultimateScript = GET_SCRIPT(fangUltimateGameObject, UltimateFang);
		ultimateCooldownRemaining = ultimateCooldown;
	}

}

void Fang::IncreaseUltimateCounter()
{
	ultimateCooldownRemaining++;
	Debug::Log("IncreaseUltimateCounter");
}

void Fang::GetHit(float damage_) {

	if (!dashing) {
		Player::GetHit(damage_);
	}

}

void Fang::InitDash() {
	if (CanDash()) {
		if (movementInputDirection != MovementDirection::NONE) {
			dashDirection = GetDirection();
			dashMovementDirection = movementInputDirection;
		} else {
			dashDirection = facePointDir;
		}

		dashCooldownRemaining = dashCooldown;
		dashRemaining = dashDuration;
		dashInCooldown = true;
		dashing = true;
		if (agent) {
			agent->SetMaxSpeed(dashSpeed);
		}

		if (playerAudios[static_cast<int>(AudioPlayer::FIRST_ABILITY)]) {
			playerAudios[static_cast<int>(AudioPlayer::FIRST_ABILITY)]->Play();
		}
	}

	if (hudControllerScript) {
		hudControllerScript->SetCooldownRetreival(HUDController::Cooldowns::FANG_SKILL_1);
	}
}

void Fang::Dash() {
	if (dashing) {
		float3 newPosition = playerMainTransform->GetGlobalPosition();
		newPosition += dashSpeed * dashDirection;
		agent->SetMoveTarget(newPosition, false);
	}
}

bool Fang::CanDash() {
	return isAlive && !dashing && !dashInCooldown && !ultimateOn;
}

void Fang::CheckCoolDowns(bool noCooldownMode) {

	//Dash Cooldown
	if (dashInCooldown) {
		if (noCooldownMode || dashCooldownRemaining <= 0.f) {
			dashCooldownRemaining = 0.f;
			dashInCooldown = false;
			dashMovementDirection = MovementDirection::NONE;
		} else {
			dashCooldownRemaining -= Time::GetDeltaTime();
		}
	}

	//Dash duration
	if (dashing) {
		if (dashRemaining <= 0.f) {
			dashRemaining = 0.f;
			dashing = false;
			agent->SetMaxSpeed(movementSpeed);
		} else {
			dashRemaining -= Time::GetDeltaTime();
		}
	}

	//Attack Cooldown
	if (shootingOnCooldown) {
		if (attackCooldownRemaining <= 0.f) {
			attackCooldownRemaining = 0.f;
			shootingOnCooldown = false;
		} else {
			attackCooldownRemaining -= Time::GetDeltaTime();
		}
	}

	//Ultimate Cooldown
	if (ultimateInCooldown) {
		if (noCooldownMode || ultimateCooldownRemaining >= ultimateCooldown) {
			ultimateCooldownRemaining = ultimateCooldown;
			ultimateInCooldown = false;
			Debug::Log("Ultiante ON");
		}
	}
}

void Fang::OnAnimationFinished() {
	if (compAnimation->GetCurrentState()) {
		if (compAnimation->GetCurrentState()->name == "Ultimate") {
			compAnimation->SendTrigger(states[22] + states[0]);
			ultimateOn = false;
			movementSpeed = oldMovementSpeed;
			ultimateScript->EndUltimate();
		}
	}
}

float Fang::GetRealDashCooldown() {
	return 1.0f - (dashCooldownRemaining / dashCooldown);
}

float Fang::GetRealUltimateCooldown()
{
	return (ultimateCooldownRemaining / ultimateCooldown);
}

bool Fang::CanShoot() {
	return !shootingOnCooldown  && !ultimateOn;
}

void Fang::Shoot() {
	if (CanShoot()) {
		shootingOnCooldown = true;
		attackCooldownRemaining = 1.f / attackSpeed;
		if (playerAudios[static_cast<int>(AudioPlayer::SHOOT)]) {
			playerAudios[static_cast<int>(AudioPlayer::SHOOT)]->Play();
		}

		ComponentTransform* shootingGunTransform = nullptr;
		if (rightShot) {
			if(compAnimation->GetCurrentState()) compAnimation->SendTriggerSecondary(compAnimation->GetCurrentState()->name + states[11]);
			shootingGunTransform = rightGunTransform;
		} else {
			if (compAnimation->GetCurrentState()) compAnimation->SendTriggerSecondary(compAnimation->GetCurrentState()->name + states[10]);
			shootingGunTransform = leftGunTransform;
		}
		if (trail && bullet && shootingGunTransform) {
			GameplaySystems::Instantiate(bullet, shootingGunTransform->GetGlobalPosition(), playerMainTransform->GetGlobalRotation());
			GameplaySystems::Instantiate(trail, shootingGunTransform->GetGlobalPosition(), playerMainTransform->GetGlobalRotation());
		}
	}
}

void Fang::PlayAnimation() {
	if (!compAnimation) return;

	int dashAnimation = 0;
	if (dashing) {
		dashAnimation = 4;
		movementInputDirection = dashMovementDirection;
	}

	if (compAnimation->GetCurrentState()) {
		if (ultimateOn || compAnimation->GetCurrentState()->name == states[22]) {
			if (compAnimation->GetCurrentState()->name != states[22]) {
				compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[22]);
			}
		}
		else if (movementInputDirection == MovementDirection::NONE) {
			if (!isAlive) {
				if (compAnimation->GetCurrentState()->name != states[9]) {
					compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[9]);
					if (compAnimation->GetCurrentStateSecondary()) {
						if (compAnimation->GetCurrentStateSecondary()->name == "RightShot") {
							compAnimation->SendTriggerSecondary("RightShotDeath");
						}
						else if (compAnimation->GetCurrentStateSecondary()->name == "LeftShot") {
							compAnimation->SendTriggerSecondary("LeftShotDeath");
						}
					}
				}
			} else {
				if (compAnimation->GetCurrentState()->name != states[0]) {
					compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[0]);
				}
			}
		} else {
			if (compAnimation->GetCurrentState()->name != states[GetMouseDirectionState() + dashAnimation]) {
				compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[GetMouseDirectionState() + dashAnimation]);
			}
		}
	} 
}

void Fang::ActiveUltimate()
{
	if (CanUltimate()) {
		Debug::Log("Ultimate Fang");
		ultimateCooldownRemaining = 0;
		ultimateOn = true;
		ultimateInCooldown = true;
		ultimateScript->StartUltiamte();

		oldMovementSpeed = movementSpeed;
		movementSpeed = ultimateMovementSpeed;

		if (playerAudios[static_cast<int>(AudioPlayer::THIRD_ABILITY)]) {
			playerAudios[static_cast<int>(AudioPlayer::THIRD_ABILITY)]->Play();
		}
	}
}

bool Fang::CanUltimate()
{
	return ultimateCooldownRemaining >= ultimateCooldown && !ultimateOn;
}

void Fang::Update(bool lockMovement, bool lockRotation) {
	if (isAlive) {
		Player::Update(dashing, dashing || ultimateOn);
		if (Input::GetMouseButtonDown(2)) {
			InitDash();
		}
		if (!dashing) {
			if (Input::GetMouseButtonDown(0)) Shoot();
			if (Input::GetKeyCodeUp(Input::KEYCODE::KEY_E)) {
				ActiveUltimate();
			}
		}
		Dash();

	} else {
		if (agent) agent->RemoveAgentFromCrowd();
		movementInputDirection = MovementDirection::NONE;
	}
	PlayAnimation();
}



