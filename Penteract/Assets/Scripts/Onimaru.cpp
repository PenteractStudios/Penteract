#include "Onimaru.h"
#include "OnimaruBullet.h"
#include "CameraController.h"

bool Onimaru::CanShoot() {
	return !shooting;
}

void Onimaru::Shoot() {
	if (CanShoot()) {
		shooting = true;
		attackCooldownRemaining = 1.f / attackSpeed;
		if (playerAudios[static_cast<int>(AudioPlayer::SHOOT)]) {
			playerAudios[static_cast<int>(AudioPlayer::SHOOT)]->Play();
		}
		if (bullet) {
			GameObject* bulletInstance = GameplaySystems::Instantiate(bullet, gunTransform->GetGlobalPosition(), Quat(0.0f, 0.0f, 0.0f, 0.0f));
			if (bulletInstance) {
				OnimaruBullet* onimaruBulletScript = GET_SCRIPT(bulletInstance, OnimaruBullet);
				if (onimaruBulletScript) onimaruBulletScript->SetOnimaruDirection(gunTransform->GetGlobalRotation());
			}
		}
	}
}

void Onimaru::PlayAnimation() {
	if (!compAnimation) return;
	if (movementInputDirection == MovementDirection::NONE) {
		if (!isAlive) {
			if (compAnimation->GetCurrentState()->name != states[9]) {
				compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[9]);
				compAnimation->SendTriggerSecondary("ShootingDeath");
			}
		} else {
			if (compAnimation->GetCurrentState()->name != states[0]) {
				compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[0]);
			}
		}
	} else {
		if (compAnimation->GetCurrentState()->name != states[GetMouseDirectionState()]) {
			compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[GetMouseDirectionState()]);
		}
	}
}

void Onimaru::CheckCoolDowns(bool noCooldownMode) {
	//AttackCooldown
	if (attackCooldownRemaining <= 0.f) {
		attackCooldownRemaining = 0.f;
		shooting = false;
	} else {
		attackCooldownRemaining -= Time::GetDeltaTime();
	}
}

void Onimaru::Init(UID onimaruUID, UID onimaruBulletUID, UID onimaruGunUID, UID cameraUID, UID canvasUID) {
	SetTotalLifePoints(lifePoints);
	characterGameObject = GameplaySystems::GetGameObject(onimaruUID);

	if (characterGameObject && characterGameObject->GetParent()) {
		playerMainTransform = characterGameObject->GetParent()->GetComponent<ComponentTransform>();
		agent = characterGameObject->GetParent()->GetComponent<ComponentAgent>();
		compAnimation = characterGameObject->GetComponent<ComponentAnimation>();

		GameObject* cameraAux = GameplaySystems::GetGameObject(cameraUID);
		if (cameraAux) {
			lookAtMouseCameraComp = cameraAux->GetComponent<ComponentCamera>();
			cameraController = GET_SCRIPT(cameraAux, CameraController);
		}

		if (agent) {
			agent->SetMaxSpeed(movementSpeed);
			agent->SetMaxAcceleration(MAX_ACCELERATION);
		}
	}

	GameObject* onimaruGun = GameplaySystems::GetGameObject(onimaruGunUID);
	if (onimaruGun) {
		gunTransform = onimaruGun->GetComponent<ComponentTransform>();
		lookAtMousePlanePosition = gunTransform->GetGlobalPosition();
	}

	bullet = GameplaySystems::GetResource<ResourcePrefab>(onimaruBulletUID);

	if (characterGameObject) {
		//Get audio sources
		int i = 0;

		for (ComponentAudioSource& src : characterGameObject->GetComponents<ComponentAudioSource>()) {
			if (i < static_cast<int>(AudioPlayer::TOTAL)) playerAudios[i] = &src;
			i++;
		}

	}
}

void Onimaru::Update(bool lockMovement) {
	if (isAlive) {
		Player::Update();
		if (Input::GetMouseButtonDown(0)) {
			if (compAnimation) {
				compAnimation->SendTriggerSecondary(compAnimation->GetCurrentState()->name + states[10]);
			}
		} else if (Input::GetMouseButtonRepeat(0)) {
			Shoot();
		} else if (Input::GetMouseButtonUp(0)) {
			if (compAnimation) {
				compAnimation->SendTriggerSecondary(states[10] + compAnimation->GetCurrentState()->name);
			}
		}
	} else {
		if (agent) agent->RemoveAgentFromCrowd();
		movementInputDirection = MovementDirection::NONE;
	}
	PlayAnimation();
}
