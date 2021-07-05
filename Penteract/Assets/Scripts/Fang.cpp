#include "Fang.h"
#include "GameplaySystems.h"
#include "HUDController.h"
#include "CameraController.h"

void Fang::Init(UID fangUID, UID trailGunUID, UID trailDashUID, UID leftGunUID, UID rightGunUID, UID rightBulletUID, UID leftBulletUID, UID cameraUID, UID canvasUID) {
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

		GameObject* trailAux = GameplaySystems::GetGameObject(trailDashUID);
		if (trailAux) {
			trailDash = trailAux->GetComponent<ComponentTrail>();
			trailDash->Stop();
		}
		trailGun = GameplaySystems::GetResource<ResourcePrefab>(trailGunUID);
		GameObject* rightBulletAux = GameplaySystems::GetGameObject(rightBulletUID);
		if (rightBulletAux) {
			rightBullet = rightBulletAux->GetComponent<ComponentParticleSystem>();
			rightBullet->Stop();
		}
		GameObject* leftBulletAux = GameplaySystems::GetGameObject(leftBulletUID);
		if (leftBulletAux) {
			leftBullet = leftBulletAux->GetComponent<ComponentParticleSystem>();
			leftBullet->Stop();
		}
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
		}
		else {
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
		trailDuration = 0.2f;
		trailDash->Play();
		float3 newPosition = playerMainTransform->GetGlobalPosition();
		newPosition += dashSpeed * dashDirection;
		agent->SetMoveTarget(newPosition, false);
	}
	else {
		trailDelay();
		//trailDash->Stop();
	}
}
void Fang::trailDelay() {
	if (trailDuration >= 0) {
		trailDuration -= Time::GetDeltaTime();
	}
	else {
		trailDash->Stop();
	}
}
bool Fang::CanDash() {
	return isAlive && !dashing && !dashInCooldown;
}

void Fang::CheckCoolDowns(bool noCooldownMode) {
	//Dash Cooldown
	if (dashInCooldown) {
		if (noCooldownMode || dashCooldownRemaining <= 0.f) {
			dashCooldownRemaining = 0.f;
			dashInCooldown = false;
			dashMovementDirection = MovementDirection::NONE;
		}
		else {
			dashCooldownRemaining -= Time::GetDeltaTime();
		}
	}

	//Dash duration
	if (dashing) {
		if (dashRemaining <= 0.f) {
			dashRemaining = 0.f;
			dashing = false;
			agent->SetMaxSpeed(movementSpeed);
		}
		else {
			dashRemaining -= Time::GetDeltaTime();
		}
	}

	//Attack Cooldown
	if (shootingOnCooldown) {
		if (attackCooldownRemaining <= 0.f) {
			attackCooldownRemaining = 0.f;
			shootingOnCooldown = false;
		}
		else {
			attackCooldownRemaining -= Time::GetDeltaTime();
		}
	}
}

void Fang::OnAnimationFinished() {
	//TODO use if necesary
}

float Fang::GetRealDashCooldown() {
	return 1.0f - (dashCooldownRemaining / dashCooldown);
}

bool Fang::CanShoot() {
	return !shootingOnCooldown;
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
			if (compAnimation->GetCurrentState()) compAnimation->SendTriggerSecondary(compAnimation->GetCurrentState()->name + states[11]);
			shootingGunTransform = rightGunTransform;
		}
		else {
			if (compAnimation->GetCurrentState()) compAnimation->SendTriggerSecondary(compAnimation->GetCurrentState()->name + states[10]);
			shootingGunTransform = leftGunTransform;
		}
		if (trailGun && rightBullet && leftBullet && shootingGunTransform) {
			if (rightShot) {
				//rightBullet->Play();
			//	rightBullet->SetParticlesPerSecond(float2(10.0f, 10.0f));
				//leftBullet->SetParticlesPerSecond(float2(0.0f, 0.0f));
			}
			else {
				//leftBullet->Play();
				//rightBullet->SetParticlesPerSecond(float2(0.0f, 0.0f));
				//leftBullet->SetParticlesPerSecond(float2(10.0f, 10.0f));
			}
			GameObject* bullet = GameplaySystems::Instantiate(trailGun, shootingGunTransform->GetGlobalPosition(), (playerMainTransform->GetGlobalMatrix().RotatePart() * float3x3::FromEulerXYZ(pi/2, 0.0f, 0.0f)).ToQuat());
			if (bullet->GetComponent<ComponentParticleSystem>()) {
				//	float3 newWorldRotation = (bullet->GetComponent<ComponentTransform>()->GetGlobalMatrix().RotatePart() * float3x3::FromEulerXYZ(90.0f, 0.0f, 0.0f)).ToEulerXYZ();
				//bullet->GetComponent<ComponentTransform>()->SetGlobalRotation(newWorldRotation);
				bullet->GetComponent<ComponentParticleSystem>()->Play();
			}
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
		if (movementInputDirection == MovementDirection::NONE) {
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
			}
			else {
				if (compAnimation->GetCurrentState()->name != states[0]) {
					compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[0]);
				}
			}
		}
		else {
			if (compAnimation->GetCurrentState()->name != states[GetMouseDirectionState() + dashAnimation]) {
				compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[GetMouseDirectionState() + dashAnimation]);
			}
		}
	}
}

void Fang::Update(bool lockMovement) {
	if (isAlive) {
		Player::Update(dashing);
		if (Input::GetMouseButtonDown(2)) {
			InitDash();
		}
		if (!dashing) {
			if (Input::GetMouseButtonDown(0)) Shoot();
		}
		Dash();
	}
	else {
		if (agent) agent->RemoveAgentFromCrowd();
		movementInputDirection = MovementDirection::NONE;
	}
	PlayAnimation();
}