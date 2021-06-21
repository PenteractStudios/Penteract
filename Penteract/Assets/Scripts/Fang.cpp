#include "Fang.h"
#include "GameplaySystems.h"
#include "HUDController.h"

Fang::Fang(int lifePoints_, float movementSpeed_, int damageHit_, float attackSpeed_, UID fangUID, UID trailUID, UID leftGunUID, UID rightGunUID,
	UID bulletUID)
{
	attackSpeed = attackSpeed_;
	lifePoints = lifePoints_;
	movementSpeed = movementSpeed_;
	damageHit = damageHit_;
	SetTotalLifePoints(lifePoints);

	fang = GameplaySystems::GetGameObject(fangUID);

	if (fang && fang->GetParent()) {
		characterTransform = fang->GetParent()->GetComponent<ComponentTransform>();
		agent = fang->GetParent()->GetComponent<ComponentAgent>();
		compAnimation = fang->GetComponent<ComponentAnimation>();

		//right gun
		GameObject* gunAux = GameplaySystems::GetGameObject(rightGunUID);
		if (gunAux) rightGunTransform = gunAux->GetComponent<ComponentTransform>();
		//left gun
		gunAux = GameplaySystems::GetGameObject(leftGunUID);
		if (gunAux) leftGunTransform = gunAux->GetComponent<ComponentTransform>();
		
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


}

void Fang::InitDash(MovementDirection md) {

	if (CanDash()) {
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
		if (audiosPlayer[static_cast<int>(AudioPlayer::FIRST_ABILITY)]) {
			audiosPlayer[static_cast<int>(AudioPlayer::FIRST_ABILITY)]->Play();
		}
	}
	//para animacion de volver a tener la habilidad
	//if (hudControllerScript) {
	//	hudControllerScript->SetCooldownRetreival(HUDController::Cooldowns::FANG_SKILL_1);
	//}
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
		if (audiosPlayer[static_cast<int>(AudioPlayer::SHOOT)]) {
			audiosPlayer[static_cast<int>(AudioPlayer::SHOOT)]->Play();
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

