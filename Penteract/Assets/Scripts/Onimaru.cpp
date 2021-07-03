#include "Onimaru.h"
#include "OnimaruBullet.h"
#include "CameraController.h"

bool Onimaru::CanShoot() {
	return !shootingOnCooldown;
}

void Onimaru::Shoot() {
	if (!gunTransform || !transformForUltimateProjectileOrigin) return;
	if (CanShoot()) {
		shootingOnCooldown = true;
		attackCooldownRemaining = 1.f / attackSpeed;
		if (playerAudios[static_cast<int>(AudioPlayer::SHOOT)]) {
			playerAudios[static_cast<int>(AudioPlayer::SHOOT)]->Play();
		}
		if (bullet) {
			float3 projectilePos = ultimateInUse ? transformForUltimateProjectileOrigin->GetGlobalPosition() : gunTransform->GetGlobalPosition();
			GameObject* bulletInstance = GameplaySystems::Instantiate(bullet, projectilePos, Quat(0.0f, 0.0f, 0.0f, 0.0f));
			if (bulletInstance) {
				OnimaruBullet* onimaruBulletScript = GET_SCRIPT(bulletInstance, OnimaruBullet);
				if (onimaruBulletScript) {
					onimaruBulletScript->SetOnimaruDirection(GetSlightRandomSpread(0, maxBulletSpread) * gunTransform->GetGlobalRotation());
				}
			}
		}
	}
}

void Onimaru::PlayAnimation() {
	if (!compAnimation) return;
	if (ultimateInUse || !isAlive) return; //Ultimate will block out all movement and idle from happening

	if (movementInputDirection == MovementDirection::NONE) {
		//Primery state machine idle when alive, without input movement
		if (compAnimation->GetCurrentState()->name != states[IDLE]) {
			compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[static_cast<int>(IDLE)]);
		}
	} else {
		//If Movement is found, Primary state machine will be in charge of getting movement animations
		if (compAnimation->GetCurrentState()->name != (states[GetMouseDirectionState()])) {
			compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[GetMouseDirectionState()]);
		}
	}
}

void Onimaru::StartUltimate() {
	//TODO COOLDOWN MANAGEMENT
	if (!compAnimation) return;

	if (compAnimation->GetCurrentStateSecondary()->name != compAnimation->GetCurrentState()->name) {
		compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + compAnimation->GetCurrentState()->name);
	}

	compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[ULTI_INTRO]);

	ultimateChargePoints = 0;
	//ultimateTimeRemaining = ultimateTotalTime;
	orientationSpeed = ultimateRotationSpeed;
	attackSpeed = ultimateAttackSpeed;
	movementInputDirection = MovementDirection::NONE;
	Player::MoveTo();
	ultimateInUse = true;
}

void Onimaru::FinishUltimate() {
	//TODO COOLDOWN MANAGEMENT
	ultimateTimeRemaining = 0;
	orientationSpeed = -1;
	attackSpeed = originalAttackSpeed;
	ultimateInUse = false;
	compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[IDLE]);

}

void Onimaru::CheckCoolDowns(bool noCooldownMode) {
	//AttackCooldown
	if (attackCooldownRemaining <= 0.f) {
		attackCooldownRemaining = 0.f;
		shootingOnCooldown = false;
	} else {
		attackCooldownRemaining -= Time::GetDeltaTime();
	}
}

bool Onimaru::CanSwitch() const {
	return ultimateTimeRemaining <= 0 && !ultimateInUse;
}

void Onimaru::Init(UID onimaruUID, UID onimaruBulletUID, UID onimaruGunUID, UID cameraUID, UID canvasUID, UID onimaruTransformForUltimateProjectileOriginUID) {
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

	originalAttackSpeed = attackSpeed;


	GameObject* onimaruGun = GameplaySystems::GetGameObject(onimaruGunUID);
	if (onimaruGun) {
		gunTransform = onimaruGun->GetComponent<ComponentTransform>();
		lookAtMousePlanePosition = gunTransform->GetGlobalPosition();
	}

	GameObject* onimaruObjForUltProj = GameplaySystems::GetGameObject(onimaruTransformForUltimateProjectileOriginUID);
	if (onimaruObjForUltProj) {
		transformForUltimateProjectileOrigin = onimaruObjForUltProj->GetComponent<ComponentTransform>();
	}

	if (transformForUltimateProjectileOrigin == nullptr) {
		transformForUltimateProjectileOrigin = gunTransform;
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
void Onimaru::OnAnimationFinished() {
	if (!compAnimation)return;
	if (ultimateInUse) {
		ultimateTimeRemaining = ultimateTotalTime;
		compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[static_cast<int>(ULTI_LOOP)]);
	}
}

void Onimaru::OnDeath() {
	if (compAnimation->GetCurrentState()->name != states[DEATH]) {
		compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + compAnimation->GetCurrentState()->name);
		compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[static_cast<int>(DEATH)]);
	}
	ultimateInUse = blastInUse = shieldInUse = false;
}


void Onimaru::Update(bool lockMovement, bool lockOrientation) {
	if (isAlive) {
		Player::Update(ultimateInUse, false);

		if (!ultimateInUse) {
			ultimateChargePoints = ultimateChargePointsTotal;
			if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_E)) {
				if (ultimateChargePoints >= ultimateChargePointsTotal) {
					StartUltimate();
				}
			}
		} else {
			if (ultimateTimeRemaining > 0) {
				ultimateTimeRemaining -= Time::GetDeltaTime();
				Shoot();
				if (ultimateTimeRemaining <= 0) {
					FinishUltimate();
				}
			}
		}

		if (!ultimateInUse && !blastInUse) {
			if (Input::GetMouseButtonDown(0)) {
				if (compAnimation) {
					if (!shieldInUse) {
						compAnimation->SendTriggerSecondary(compAnimation->GetCurrentState()->name + states[static_cast<int>(SHOOTING)]);
					} else {
						compAnimation->SendTriggerSecondary(compAnimation->GetCurrentState()->name + states[static_cast<int>(SHOOTSHIELD)]);
					}
				}
				shooting = true;
			} else if (Input::GetMouseButtonRepeat(0)) {
				Shoot();
			} else if (Input::GetMouseButtonUp(0)) {
				if (compAnimation) {
					compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + compAnimation->GetCurrentState()->name);
				}
				shooting = false;
			}
		}
		//TODO Ability handling
		//Whenever an ability starts being used, make sure that as well as setting the secondary trigger to whatever, if (shooting was true, it must be turned to false)

	} else {
		if (agent) agent->RemoveAgentFromCrowd();
		movementInputDirection = MovementDirection::NONE;
	}
	PlayAnimation();
}

Quat Onimaru::GetSlightRandomSpread(float minValue, float maxValue) const {

	float sign = rand() % 2 < 1 ? 1.0f : -1.0f;

	float4 axis = float4(gunTransform->GetUp(), 1);

	float randomAngle = static_cast<float>((rand() % static_cast<int>(maxValue * 100))) / 100 + minValue;

	Quat result = Quat(0, 0, 0, 1);

	result.SetFromAxisAngle(axis, DEGTORAD * randomAngle * sign);

	return result;
}