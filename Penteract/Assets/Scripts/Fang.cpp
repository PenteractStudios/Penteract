#include "Fang.h"
#include "GameplaySystems.h"
#include "HUDController.h"
#include "UltimateFang.h"
#include "CameraController.h"

void Fang::Init(UID fangUID, UID trailUID, UID leftGunUID, UID rightGunUID, UID bulletUID, UID cameraUID, UID canvasUID, UID EMPUID, UID fangUltimateUID)
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
	EMP = GameplaySystems::GetGameObject(EMPUID);
	if (EMP) {
		ComponentSphereCollider* sCollider = EMP->GetComponent<ComponentSphereCollider>();
		if (sCollider) sCollider->radius = EMPRadius;
	}

	GameObject* fangUltimateGameObject = GameplaySystems::GetGameObject(fangUltimateUID);
	if (fangUltimateGameObject) {
		ultimateScript = GET_SCRIPT(fangUltimateGameObject, UltimateFang);
		ultimateCooldownRemaining = ultimateCooldown;
	}
}

bool Fang::CanSwitch() const {
	if (!EMP) return false;
	return !EMP->IsActive() && !ultimateOn;
}

void Fang::IncreaseUltimateCounter()
{
	ultimateCooldownRemaining++;
	Debug::Log(std::to_string((int)ultimateCooldownRemaining).c_str());
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
	return !dashing && !dashInCooldown && !EMP->IsActive() && !ultimateOn;
}

void Fang::ActivateEMP() {
	if (EMP && CanEMP()) {
		EMP->Enable();
		EMPCooldownRemaining = EMPCooldown;
		EMPInCooldown = true;

		if (playerAudios[static_cast<int>(AudioPlayer::SECOND_ABILITY)]) {
			playerAudios[static_cast<int>(AudioPlayer::SECOND_ABILITY)]->Play();
		}
		if (hudControllerScript) {
			hudControllerScript->SetCooldownRetreival(HUDController::Cooldowns::FANG_SKILL_2);
		}
	}
}

bool Fang::CanEMP() {
	return !EMP->IsActive() && !EMPInCooldown && !dashing;
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

	//EMP Cooldown
	if (EMPInCooldown) {
		if (noCooldownMode || EMPCooldownRemaining <= 0.f) {
			EMPCooldownRemaining = 0.f;
			EMPInCooldown = false;
		}
		else {
			EMPCooldownRemaining -= Time::GetDeltaTime();
		}
	}

	//Ultimate Cooldown
	if (ultimateInCooldown) {
		if (noCooldownMode || ultimateCooldownRemaining >= ultimateCooldown) {
			ultimateCooldownRemaining = ultimateCooldown;
			ultimateInCooldown = false;
		}
	}
}

void Fang::OnAnimationFinished() {
	if (compAnimation) {
		if (compAnimation->GetCurrentState()) {
			if (compAnimation->GetCurrentState()->name == "EMP") {
				compAnimation->SendTrigger(states[21] + states[0]);
				EMP->Disable();
			} else if (compAnimation->GetCurrentState()->name == "Ultimate") {
				compAnimation->SendTrigger(states[22] + states[0]);
				ultimateOn = false;
				movementSpeed = oldMovementSpeed;
				ultimateScript->EndUltimate();
			}
		}
	}
}

void Fang::OnAnimationSecondaryFinished() {
}

float Fang::GetRealDashCooldown() {
	return 1.0f - (dashCooldownRemaining / dashCooldown);
}

float Fang::GetRealEMPCooldown()
{
	return 1.0f - (EMPCooldownRemaining / EMPCooldown);
}

float Fang::GetRealUltimateCooldown()
{
	return (ultimateCooldownRemaining / (float)ultimateCooldown);
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
	if (!EMP) return;

	int dashAnimation = 0;
	if (dashing) {
		dashAnimation = 4;
		movementInputDirection = dashMovementDirection;
	}
	if (EMP->IsActive()) movementInputDirection = MovementDirection::NONE;

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
				if (compAnimation->GetCurrentState()->name != states[0] && compAnimation->GetCurrentState()->name != states[21]) {
					compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[0]);
				}
				if (compAnimation->GetCurrentState()->name == states[0] && EMP->IsActive()) {
					compAnimation->SendTrigger(states[0] + states[21]);
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
		ultimateCooldownRemaining = 0;
		ultimateOn = true;
		ultimateInCooldown = true;
		ultimateScript->StartUltiamte();

		oldMovementSpeed = movementSpeed;
		movementSpeed = ultimateMovementSpeed;

		if (playerAudios[static_cast<int>(AudioPlayer::THIRD_ABILITY)]) {
			playerAudios[static_cast<int>(AudioPlayer::THIRD_ABILITY)]->Play();
		}

		if (hudControllerScript) {
			hudControllerScript->SetCooldownRetreival(HUDController::Cooldowns::FANG_SKILL_3);
		}
	}
}

bool Fang::CanUltimate()
{
	return ultimateCooldownRemaining >= ultimateCooldown && !ultimateOn;
}

void Fang::Update(bool lockMovement, bool lockRotation) {
	if (isAlive) {
		if (EMP) {
			Player::Update(dashing || EMP->IsActive(), dashing || EMP->IsActive() || ultimateOn);
			if (Input::GetMouseButtonDown(2) && !EMP->IsActive()) {
				InitDash();
			}
			if (!dashing && !EMP->IsActive()) {
				if (Input::GetMouseButtonDown(0)) Shoot();
			}
			Dash();
			if (Input::GetKeyCodeDown(Input::KEY_Q)) {
				ActivateEMP();
			}
			if (Input::GetKeyCodeUp(Input::KEYCODE::KEY_E)) {
				ActiveUltimate();
			}
		}		
	} 
	else {
		if (agent) agent->RemoveAgentFromCrowd();
		movementInputDirection = MovementDirection::NONE;
	}
	PlayAnimation();
}



