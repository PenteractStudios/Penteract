#include "Fang.h"
#include "GameplaySystems.h"

void Fang::InitDash(MovementDirection md) {
	if (CanDash()) {

		if (hudControllerScript) {
			hudControllerScript->SetCooldownRetreival(HUDController::Cooldowns::FANG_SKILL_1);
		}

		if (md != MovementDirection::NONE) {
			dashDirection = GetDirection(md);
			dashMovementDirection = md;
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
		if (audios[static_cast<int>(AudioType::DASH)]) {
			audios[static_cast<int>(AudioType::DASH)]->Play();
		}
	}
}

void Fang::Dash() {
	if (dashing) {
		float3 newPosition = characterTransform->GetGlobalPosition();
		newPosition += dashSpeed * dashDirection;
		agent->SetMoveTarget(newPosition, false);
	}
}

bool Fang::CanDash() {
	return !dashing && !dashInCooldown;
}

void Fang::CheckCoolDowns(bool noCooldownMode) {
	//Dash Cooldown
	if (noCooldownMode || dashCooldownRemaining <= 0.f) {
		dashCooldownRemaining = 0.f;
		dashInCooldown = false;
		dashMovementDirection = MovementDirection::NONE;
	}
	else {
		dashCooldownRemaining -= Time::GetDeltaTime();
	}

	//Dash duration
	if (dashRemaining <= 0.f) {
		dashRemaining = 0.f;
		dashing = false;
		agent->SetMaxSpeed(movementSpeed);
	}
	else {
		dashRemaining -= Time::GetDeltaTime();
	}

	//AttackCooldown
	if (attackCooldownRemaining <= 0.f) {
		attackCooldownRemaining = 0.f;
		shooting = false;
	}
	else {
		attackCooldownRemaining -= Time::GetDeltaTime();
	}
}

void Fang::Update(MovementDirection md) {
	if (Input::GetMouseButtonDown(2)) {
		InitDash(md);
	}
	if (!dashing) {
		if (Input::GetMouseButtonDown(0)) Shoot();
	}
}

void Fang::Shoot() {
	if (CanShoot()) {
		shooting = true;
			attackCooldownRemaining = 1.f / attackSpeed;
			if (audios[static_cast<int>(AudioType::SHOOT)]) {
				audios[static_cast<int>(AudioType::SHOOT)]->Play();
			}

			ComponentTransform* shootingGunTransform = nullptr;
			if (rightShot) {
				compAnimation->SendTriggerSecondary(compAnimation->GetCurrentState()->name + states[12]);
				shootingGunTransform = rightGunTransform;
			}
			else {
				compAnimation->SendTriggerSecondary(compAnimation->GetCurrentState()->name + states[11]);
				shootingGunTransform = leftGunTransform;
			}
			if (trail && bullet && shootingGunTransform) {
				GameplaySystems::Instantiate(bullet, shootingGunTransform->GetGlobalPosition(), characterTransform->GetGlobalRotation());
				GameplaySystems::Instantiate(trail, shootingGunTransform->GetGlobalPosition(), characterTransform->GetGlobalRotation());
			}
		}
}

