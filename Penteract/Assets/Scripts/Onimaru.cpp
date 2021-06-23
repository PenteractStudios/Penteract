#include "Onimaru.h"

bool Onimaru::CanShoot() {
	return !shooting;
}

void Onimaru::Shoot() {
	if (CanShoot()) {
		shooting = true;
		attackCooldownRemaining = 1.f / attackSpeed;
		if (audiosPlayer[static_cast<int>(AudioPlayer::SHOOT)]) {
			audiosPlayer[static_cast<int>(AudioPlayer::SHOOT)]->Play();
		}
		/*if (onimaruBullet) {
			GameObject* bullet = GameplaySystems::Instantiate(onimaruBullet, onimaruGunTransform->GetGlobalPosition(), Quat(0.0f, 0.0f, 0.0f, 0.0f));
			if (bullet) {
				onimaruBulletcript = GET_SCRIPT(bullet, OnimaruBullet);
				if (onimaruBulletcript) onimaruBulletcript->SetOnimaruDirection(onimaruGunTransform->GetGlobalRotation());
			}
		}*/
	}
}

void Onimaru::PlayAnimation() {
	if (!compAnimation) return;
	if (movementInputDirection == MovementDirection::NONE) {
		if (isAlive) {
			if (compAnimation->GetCurrentState()->name != states[9]) {
				compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[9]);
				compAnimation->SendTriggerSecondary("ShootingDeath");
			}
		}
		else {
			if (compAnimation->GetCurrentState()->name != states[0]) {
				compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[0]);
			}
		}
	}
	else {
		if (compAnimation->GetCurrentState()->name != states[GetMouseDirectionState()]) {
			compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[GetMouseDirectionState()]);
		}
	}
}

void Onimaru::Init(int lifePoints_, float movementSpeed_, int damageHit_, float attackSpeed_, UID onimaruUID, UID onimaruBulletUID, UID onimaruGunUID, UID cameraUID)
{
	attackSpeed = attackSpeed_;
	lifePoints = lifePoints_;
	movementSpeed = movementSpeed_;
	damageHit = damageHit_;
	SetTotalLifePoints(lifePoints);
	characterGameObject = GameplaySystems::GetGameObject(onimaruUID);

	if (characterGameObject && characterGameObject->GetParent()) {
		playerMainTransform = characterGameObject->GetParent()->GetComponent<ComponentTransform>();
		agent = characterGameObject->GetParent()->GetComponent<ComponentAgent>();
		compAnimation = characterGameObject->GetComponent<ComponentAnimation>();

		GameObject* cameraAux = GameplaySystems::GetGameObject(cameraUID);
		if (cameraAux) {
			lookAtMouseCameraComp = cameraAux->GetComponent<ComponentCamera>();
		}

		if (agent) {
			agent->SetMaxSpeed(movementSpeed);
			agent->SetMaxAcceleration(MAX_ACCELERATION);
		}
	}

	GameObject* onimaruGun = GameplaySystems::GetGameObject(onimaruGunUID);
	if (onimaruGun) {
		gunTransform = onimaruGun->GetComponent<ComponentTransform>();
	}

	bullet = GameplaySystems::GetResource<ResourcePrefab>(onimaruBulletUID);
}

void Onimaru::Update(bool lockMovement) {
	if (isAlive) {
		Player::Update();
		if (Input::GetMouseButtonDown(0)) {
			if (compAnimation) {
				compAnimation->SendTriggerSecondary(compAnimation->GetCurrentState()->name + states[13]);
			}
		}
		else if (Input::GetMouseButtonRepeat(0)) {
			Shoot();
		}
		else if (Input::GetMouseButtonUp(0)) {
			if (compAnimation) {
				compAnimation->SendTriggerSecondary(states[13] + compAnimation->GetCurrentState()->name);
			}
		}
	}
	else {
		if (agent) agent->RemoveAgentFromCrowd();
	}
}
