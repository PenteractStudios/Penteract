#include "Fang.h"
#include "GameplaySystems.h"
#include "HUDController.h"
#include "CameraController.h"
#include "UltimateFang.h"

void Fang::Init(UID fangUID, UID trailGunUID, UID trailDashUID, UID leftGunUID, UID rightGunUID, UID rightBulletUID, UID leftBulletUID, UID cameraUID, UID canvasUID, UID dashUID, UID EMPUID, UID EMPEffectsUID, UID fangUltimateUID, UID ultimateVFXUID) {
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
		GameObject* gunAux1 = GameplaySystems::GetGameObject(leftGunUID);
		if (gunAux1) leftGunTransform = gunAux1->GetComponent<ComponentTransform>();
		lookAtMousePlanePosition = leftGunTransform->GetGlobalPosition();

		GameObject* trailAux = GameplaySystems::GetGameObject(trailDashUID);
		if (trailAux) {
			trailDash = trailAux->GetComponent<ComponentTrail>();
			if (trailDash) trailDash->Stop();
		}
		trailGun = GameplaySystems::GetResource<ResourcePrefab>(trailGunUID);

		rightBulletAux = GameplaySystems::GetGameObject(rightBulletUID);
		if (rightBulletAux) {
			rightBullet = rightBulletAux->GetComponent<ComponentParticleSystem>();

		}
		leftBulletAux = GameplaySystems::GetGameObject(leftBulletUID);
		if (leftBulletAux) {
			leftBullet = leftBulletAux->GetComponent<ComponentParticleSystem>();

		}
		if (compAnimation) {
			currentState = compAnimation->GetCurrentState();
		}

		if (agent) {
			agent->SetMaxSpeed(movementSpeed);
			agent->SetMaxAcceleration(MAX_ACCELERATION);
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
				if (i < static_cast<int>(FANG_AUDIOS::TOTAL)) fangAudios[i] = &src;
				i++;
			}
		}
		dash = GameplaySystems::GetGameObject(dashUID);

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
	GameObject* canvasGO = GameplaySystems::GetGameObject(canvasUID);
	if (canvasGO) {
		hudControllerScript = GET_SCRIPT(canvasGO, HUDController);
	}
	if (characterGameObject) {
		characterGameObject->GetComponent<ComponentCapsuleCollider>()->Enable();

		//Get audio sources
		int i = 0;

		for (ComponentAudioSource& src : characterGameObject->GetComponents<ComponentAudioSource>()) {
			if (i < static_cast<int>(FANG_AUDIOS::TOTAL)) fangAudios[i] = &src;
			i++;
		}
	}
	dash = GameplaySystems::GetGameObject(dashUID);

	EMP = GameplaySystems::GetGameObject(EMPUID);
	if (EMP) {
		ComponentSphereCollider* sCollider = EMP->GetComponent<ComponentSphereCollider>();
		if (sCollider) {
			sCollider->radius = EMPRadius;
			sCollider->Disable();
			sCollider->Enable();
		}
		EMP->Enable();
		EMP->Disable();
	}
	GameObject* EMPEffectsGO = GameplaySystems::GetGameObject(EMPEffectsUID);
	if (EMPEffectsGO) EMPEffects = EMPEffectsGO->GetComponent<ComponentParticleSystem>();

	GameObject* fangUltimateGameObject = GameplaySystems::GetGameObject(fangUltimateUID);
	if (fangUltimateGameObject) {
		ultimateScript = GET_SCRIPT(fangUltimateGameObject, UltimateFang);
		ultimateCooldownRemaining = 0;
	}

	GameObject* ultimateVFXGO = GameplaySystems::GetGameObject(ultimateVFXUID);
	if (ultimateVFXGO) ultimateVFX = ultimateVFXGO->GetComponent<ComponentParticleSystem>();
}
bool Fang::CanSwitch() const {
	if (!EMP) return false;
	return !EMP->IsActive() && !ultimateOn;
}

void Fang::IncreaseUltimateCounter() {
	if (!ultimateOn) ultimateCooldownRemaining++;
}

bool Fang::IsInstantOrientation(bool useGamepad) const {
	return !useGamepad || !Input::IsGamepadConnected(0);
}

void Fang::GetHit(float damage_) {
	if (!dashing) {
		if (cameraController) {
			cameraController->StartShake();
		}

		lifePoints -= damage_;
		if (fangAudios[static_cast<int>(FANG_AUDIOS::HIT)]) fangAudios[static_cast<int>(FANG_AUDIOS::HIT)]->Play();
		isAlive = lifePoints > 0.0f;

		if (!isAlive) {
			if (fangAudios[static_cast<int>(FANG_AUDIOS::DEATH)]) fangAudios[static_cast<int>(FANG_AUDIOS::DEATH)]->Play();
			OnDeath();
		}
	}
}

void Fang::InitDash() {
	if (CanDash()) {
		hasDashed = true;
		if (trailDash) trailDash->Play();
		if (dash && level1Upgrade) dash->Enable();
		trailDuration = dashDuration + trailDashOffsetDuration;
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

		if (fangAudios[static_cast<int>(FANG_AUDIOS::DASH)]) {
			fangAudios[static_cast<int>(FANG_AUDIOS::DASH)]->Play();
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
	} else {
		if (hasDashed) trailDelay();
	}
}

void Fang::trailDelay() {
	if (trailDuration >= 0) {
		trailDuration -= Time::GetDeltaTime();
	} else {
		hasDashed = false;
		if (trailDash) trailDash->Stop();
	}
}

bool Fang::CanDash() {
	return isAlive && !dashing && !dashInCooldown && !EMP->IsActive() && !ultimateOn;
}

void Fang::ActivateEMP() {
	if (EMP && CanEMP()) {
		if (EMPEffects) EMPEffects->PlayChildParticles();
		EMP->Enable();
		EMPCooldownRemaining = EMPCooldown;
		EMPInCooldown = true;

		if (fangAudios[static_cast<int>(FANG_AUDIOS::EMP)]) {
			fangAudios[static_cast<int>(FANG_AUDIOS::EMP)]->Play();
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
			if (dash) {
				if (dash->IsActive()) dash->Disable();
			}
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
		} else {
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

void Fang::OnAnimationEvent(StateMachineEnum stateMachineEnum, const char* eventName) {
	if (stateMachineEnum == StateMachineEnum::PRINCIPAL) {
		if (std::strcmp(eventName, "FootstepRight")) {
			if (fangAudios[static_cast<int>(FANG_AUDIOS::FOOTSTEP_RIGHT)]) {
				fangAudios[static_cast<int>(FANG_AUDIOS::FOOTSTEP_RIGHT)]->Play();
			}
		} else if (std::strcmp(eventName, "FootstepLeft")) {
			if (fangAudios[static_cast<int>(FANG_AUDIOS::FOOTSTEP_LEFT)]) {
				fangAudios[static_cast<int>(FANG_AUDIOS::FOOTSTEP_LEFT)]->Play();
			}
		}
	}
}

float Fang::GetRealDashCooldown() {
	return 1.0f - (dashCooldownRemaining / dashCooldown);
}

float Fang::GetRealEMPCooldown() {
	return 1.0f - (EMPCooldownRemaining / EMPCooldown);
}

float Fang::GetRealUltimateCooldown() {
	return (ultimateCooldownRemaining / (float)ultimateCooldown);
}

bool Fang::CanShoot() {
	return !shootingOnCooldown && !ultimateOn;
}

void Fang::Shoot() {
	if (CanShoot()) {
		shootingOnCooldown = true;
		attackCooldownRemaining = 1.f / attackSpeed;
		if (fangAudios[static_cast<int>(FANG_AUDIOS::SHOOT)]) {
			fangAudios[static_cast<int>(FANG_AUDIOS::SHOOT)]->Play();
		}

		ComponentTransform* shootingGunTransform = nullptr;
		if (rightShot) {
			if (compAnimation->GetCurrentState()) compAnimation->SendTriggerSecondary(compAnimation->GetCurrentState()->name + states[11]);
			shootingGunTransform = rightGunTransform;
		} else {
			if (compAnimation->GetCurrentState()) compAnimation->SendTriggerSecondary(compAnimation->GetCurrentState()->name + states[10]);
			shootingGunTransform = leftGunTransform;
		}
		if (trailGun && rightBullet && leftBullet && shootingGunTransform) {
			GameObject* bullet = GameplaySystems::Instantiate(trailGun, shootingGunTransform->GetGlobalPosition(), (playerMainTransform->GetGlobalMatrix().RotatePart() * float3x3::FromEulerXYZ(pi / 2, 0.0f, 0.0f)).ToQuat());
			if (bullet->GetComponent<ComponentParticleSystem>()) {
				bullet->GetComponent<ComponentParticleSystem>()->Play();
			}
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
		} else if (movementInputDirection == MovementDirection::NONE) {
			if (!isAlive) {
				if (compAnimation->GetCurrentState()->name != states[9]) {
					compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[9]);
					if (compAnimation->GetCurrentStateSecondary()) {
						if (compAnimation->GetCurrentStateSecondary()->name == "RightShot") {
							compAnimation->SendTriggerSecondary("RightShotDeath");
						} else if (compAnimation->GetCurrentStateSecondary()->name == "LeftShot") {
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

void Fang::ActiveUltimate() {
	if (CanUltimate()) {
		ultimateCooldownRemaining = 0;
		ultimateOn = true;
		ultimateInCooldown = true;
		if (ultimateVFX) ultimateVFX->PlayChildParticles();
		ultimateScript->StartUltimate();

		oldMovementSpeed = movementSpeed;
		movementSpeed = ultimateMovementSpeed;

		if (fangAudios[static_cast<int>(FANG_AUDIOS::ULTIMATE)]) {
			fangAudios[static_cast<int>(FANG_AUDIOS::ULTIMATE)]->Play();
		}

		if (hudControllerScript) {
			hudControllerScript->SetCooldownRetreival(HUDController::Cooldowns::FANG_SKILL_3);
		}
	}
}

bool Fang::CanUltimate() {
	return ultimateCooldownRemaining >= ultimateCooldown && !ultimateOn;
}

void Fang::Update(bool useGamepad, bool lockMovement, bool lockRotation) {
	if (isAlive) {
		if (EMP) {
			Player::Update(useGamepad, dashing || EMP->IsActive(), dashing || EMP->IsActive() || ultimateOn);
			if (GetInputBool(InputActions::ABILITY_1, useGamepad) && !EMP->IsActive()) {
				InitDash();
			}
			if (!dashing && !EMP->IsActive()) {
				if (GetInputBool(InputActions::SHOOT, useGamepad)) {
					Shoot();
				}
			}

			Dash();

			if (GetInputBool(InputActions::ABILITY_2, useGamepad)) {
				ActivateEMP();
			}

			if (GetInputBool(InputActions::ABILITY_3, useGamepad)) {
				ActiveUltimate();
			}
		}
	} else {
		if (agent) agent->RemoveAgentFromCrowd();
		movementInputDirection = MovementDirection::NONE;
	}
	PlayAnimation();
}