#include "Onimaru.h"
#include "OnimaruBullet.h"
#include "GameplaySystems.h"
#include "HUDController.h"
#include "Shield.h"
#include "CameraController.h"

bool Onimaru::CanShoot() {
	return !shootingOnCooldown;
}

void Onimaru::Shoot() {
	if (CanShoot()) {
		shootingOnCooldown = true;
		attackCooldownRemaining = 1.f / attackSpeed;
		if (playerAudios[static_cast<int>(AudioPlayer::SHOOT)]) {
			playerAudios[static_cast<int>(AudioPlayer::SHOOT)]->Play();
		}
		if (bullet) {
			GameObject* bulletInstance = GameplaySystems::Instantiate(bullet, gunTransform->GetGlobalPosition(), Quat(0.0f, 0.0f, 0.0f, 0.0f));
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

	if (compAnimation->GetCurrentState()) {
		if (movementInputDirection == MovementDirection::NONE) {
			//Primery state machine idle when alive, without input movement
			if (compAnimation->GetCurrentState()->name != states[IDLE]) {
				compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[static_cast<int>(IDLE)]);
			}
		}
		else {
			//If Movement is found, Primary state machine will be in charge of getting movement animations
			if (compAnimation->GetCurrentState()->name != (states[GetMouseDirectionState()])) {
				compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[GetMouseDirectionState()]);
			}
		}	
	}
}

bool Onimaru::IsShielding() {
	return shield->GetIsActive();
}

float Onimaru::GetRealShieldCooldown()
{
	float realShieldCooldown = 1.0f;
	float chargesWasted = (float)(shield->max_charges - shield->GetNumCharges()) / (float)shield->max_charges;
	if (shield->GetIsActive()) {
		realShieldCooldown = chargesWasted;
	} else if (shield->GetCoolDown() > 0) {
		realShieldCooldown = 1.0f - (shieldCooldownRemaining / (shield->GetCoolDown() / (1.0f- chargesWasted)));
	}

	return realShieldCooldown;
}

void Onimaru::CheckCoolDowns(bool noCooldownMode) {
	//AttackCooldown
	if (attackCooldownRemaining <= 0.f) {
		attackCooldownRemaining = 0.f;
		shootingOnCooldown = false;
	} else {
		attackCooldownRemaining -= Time::GetDeltaTime();
	}

	//ShieldCooldown
	if (noCooldownMode || shieldCooldownRemaining <= 0.f) {
		shieldCooldownRemaining = 0.f;
		shieldInCooldown = false;
	}
	else {
		shieldCooldownRemaining -= Time::GetDeltaTime();
	}
}

void Onimaru::OnDeath() {
	if (compAnimation->GetCurrentState()) {
		if (compAnimation->GetCurrentState()->name != states[DEATH]) {
			if(compAnimation->GetCurrentStateSecondary()) compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + compAnimation->GetCurrentState()->name);
			compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[static_cast<int>(DEATH)]);
		}
	}
	ultimateInUse = blastInUse = false;
}

void Onimaru::OnAnimationFinished() {
	//TODO use for ultimate ability
	//Other abilities may also make use of this
}

void Onimaru::Init(UID onimaruUID, UID onimaruBulletUID, UID onimaruGunUID, UID cameraUID, UID canvasUID, UID shieldUID, float maxSpread_) {
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

	maxBulletSpread = maxSpread_;

	GameObject* onimaruGun = GameplaySystems::GetGameObject(onimaruGunUID);
	if (onimaruGun) {
		gunTransform = onimaruGun->GetComponent<ComponentTransform>();
		lookAtMousePlanePosition = gunTransform->GetGlobalPosition();
	}
	GameObject* canvasGO = GameplaySystems::GetGameObject(canvasUID);
	if (canvasGO) {
		hudControllerScript = GET_SCRIPT(canvasGO, HUDController);
	}
	shieldGO = GameplaySystems::GetGameObject(shieldUID);
	if (shieldGO) {
		shield = GET_SCRIPT(shieldGO, Shield);
	}

	shieldGO->Disable();
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

bool Onimaru::CanShield() {
	return !shieldInCooldown && !shield->GetIsActive();
}

void Onimaru::InitShield() {
	if (CanShield()) {

		shield->InitShield();

		shieldInCooldown = false;
		if (agent) {
			agent->SetMaxSpeed(movementSpeed/2);
		}
		if (!shooting) {
			if (compAnimation->GetCurrentState()) {
				compAnimation->SendTriggerSecondary(compAnimation->GetCurrentState()->name + states[static_cast<int>(SHIELD)]);
			}
		}
		else {
			if (compAnimation->GetCurrentStateSecondary()) {
				compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + states[static_cast<int>(SHOOTSHIELD)]);
			}
		}
		if (playerAudios[static_cast<int>(AudioPlayer::FIRST_ABILITY)]) {
			playerAudios[static_cast<int>(AudioPlayer::FIRST_ABILITY)]->Play();
		}
		shieldGO->Enable();
	}
}

void Onimaru::FadeShield() {
	shield->FadeShield();
	shieldInCooldown = true;
	shieldCooldownRemaining = shield->GetCoolDown();
	if (agent) agent->SetMaxSpeed(movementSpeed);
	
	if (!shooting) {
		if (compAnimation->GetCurrentStateSecondary() && compAnimation->GetCurrentState()) {
			compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + compAnimation->GetCurrentState()->name);
		}
	}
	else {
		if (compAnimation->GetCurrentStateSecondary()) {
			compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + states[static_cast<int>(SHOOTING)]);
		}
	}
	if (playerAudios[static_cast<int>(AudioPlayer::DEATH)]) {
		playerAudios[static_cast<int>(AudioPlayer::DEATH)]->Play();
	}
	shieldGO->Disable();
}


void Onimaru::Update(bool lockMovement) {
	if (isAlive) {
		Player::Update();
		if (!ultimateInUse && !blastInUse) {
			if (Input::GetMouseButtonDown(2)) {
				InitShield();
			}
			if (shield->GetIsActive()) {
				if (Input::GetMouseButtonUp(2) || shield->GetNumCharges() == shield->max_charges) {
					FadeShield();
				}
			}
			if (Input::GetMouseButtonDown(0)) {
				if (compAnimation) {
					if (!shield->GetIsActive()) {
						if (compAnimation->GetCurrentState()) {
							compAnimation->SendTriggerSecondary(compAnimation->GetCurrentState()->name + states[static_cast<int>(SHOOTING)]);
						}
					}
					else {
						if (compAnimation->GetCurrentStateSecondary()) {
							compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + states[static_cast<int>(SHOOTSHIELD)]);
						}
					}
				}
				shooting = true;
			}
			else if (Input::GetMouseButtonRepeat(0)) {
					Shoot();
			} else if (Input::GetMouseButtonUp(0)) {
				if (compAnimation) {
					if (shield->GetIsActive()) {
						if (compAnimation->GetCurrentStateSecondary()) {
							compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + states[static_cast<int>(SHIELD)]);
						}
					}
					else {
						if (compAnimation->GetCurrentStateSecondary() && compAnimation->GetCurrentState()) {
							compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + compAnimation->GetCurrentState()->name);
						}
					}
					
				}
				shooting = false;
			}
		} 
		else if (Input::GetMouseButtonUp(0)) {
			if (compAnimation) {
				if (compAnimation->GetCurrentState()) {
					compAnimation->SendTriggerSecondary(states[10] + compAnimation->GetCurrentState()->name);
				}
			}
		}
	} 
	else 
	{
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