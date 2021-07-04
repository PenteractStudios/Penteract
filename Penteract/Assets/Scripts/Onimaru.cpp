#include "Onimaru.h"
#include "OnimaruBullet.h"
#include "AIMeleeGrunt.h"
#include "RangedAI.h"
#include "HUDController.h"
#include "CameraController.h"

bool Onimaru::CanShoot() {
	return !shootingOnCooldown;
}

bool Onimaru::CanBlast() {
	return !blastInCooldown;
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

void Onimaru::Blast() {
	bool releaseBlast = currentBlastDuration <= blastDuration / 2.0f ? false : true;
	if (releaseBlast) {
		for (GameObject* enemy : enemiesInMap) {
			AIMeleeGrunt* meleeScript = GET_SCRIPT(enemy, AIMeleeGrunt);
			RangedAI* rangedScript = GET_SCRIPT(enemy, RangedAI);
			if (rangedScript || meleeScript) {
				if (rightHand && playerMainTransform) {
					float3 onimaruRightArmPos = rightHand->GetGlobalPosition();
					float3 enemyPos = enemy->GetComponent<ComponentTransform>()->GetGlobalPosition();
					onimaruRightArmPos = float3(onimaruRightArmPos.x, 0.f, onimaruRightArmPos.z);
					enemyPos = float3(enemyPos.x, 0.f, enemyPos.z);
					float distance = enemyPos.Distance(onimaruRightArmPos);
					float3 direction = (enemyPos - onimaruRightArmPos).Normalized();
					if (distance <= blastDistance) {
						float angle = 0.f;
						angle = RadToDeg(playerMainTransform->GetFront().AngleBetweenNorm(direction));
						if (angle <= blastAngle / 2.0f) {
							Debug::Log("Hit. Angle: %s", std::to_string(angle));
							if (meleeScript) meleeScript->EnableBlastPushBack();
							else if (rangedScript) rangedScript->EnableBlastPushBack();
						}
						else {
							Debug::Log("Miss. Angle: %s", std::to_string(angle));
						}
					}
					else {
						if (meleeScript) {
							if (!meleeScript->IsBeingPushed()) meleeScript->DisableBlastPushBack();
						}
						else if (rangedScript) {
							if (!rangedScript->IsBeingPushed()) rangedScript->DisableBlastPushBack();
						}

					}
				}
			}
		}
		blastInCooldown = true;
		currentBlastDuration = 0.f;
	}
	else {
		currentBlastDuration += Time::GetDeltaTime();
		blastInUse = true;
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

void Onimaru::CheckCoolDowns(bool noCooldownMode) {
	//AttackCooldown
	if (attackCooldownRemaining <= 0.f) {
		attackCooldownRemaining = 0.f;
		shootingOnCooldown = false;
	} else {
		attackCooldownRemaining -= Time::GetDeltaTime();
	}

	//Blast Cooldown
	if (noCooldownMode || blastCooldownRemaining <= 0.f) {
		blastCooldownRemaining = 0.f;
		blastInCooldown = false;
	}
	else {
		if (!blastInUse) blastCooldownRemaining -= Time::GetDeltaTime();
	}
}

void Onimaru::OnDeath() {
	if (compAnimation->GetCurrentState()->name != states[DEATH]) {
		compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + compAnimation->GetCurrentState()->name);
		compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[static_cast<int>(DEATH)]);
	}
	ultimateInUse = blastInUse = shieldInUse = false;
}

void Onimaru::OnAnimationFinished() {
	//TODO use for ultimate ability
	//Other abilities may also make use of this
}

void Onimaru::OnAnimationSecondaryFinished() {
	if (compAnimation) {
		if (blastInUse) {
			blastInUse = false;
			if (compAnimation) {
				compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + compAnimation->GetCurrentState()->name);
			}
		}
	}

}

void Onimaru::Init(UID onimaruUID, UID onimaruBulletUID, UID onimaruGunUID, UID onimaruRightHandUID, UID cameraUID, UID canvasUID, float maxSpread_) {
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

	bullet = GameplaySystems::GetResource<ResourcePrefab>(onimaruBulletUID);

	if (characterGameObject) {
		//Get audio sources
		int i = 0;

		for (ComponentAudioSource& src : characterGameObject->GetComponents<ComponentAudioSource>()) {
			if (i < static_cast<int>(AudioPlayer::TOTAL)) playerAudios[i] = &src;
			i++;
		}

	}

	GameObject* canvasGO = GameplaySystems::GetGameObject(canvasUID);
	if (canvasGO) {
		hudControllerScript = GET_SCRIPT(canvasGO, HUDController);
	}

	GameObject* rightHandGO = GameplaySystems::GetGameObject(onimaruRightHandUID);
	if (rightHandGO) rightHand = rightHandGO->GetComponent<ComponentTransform>();
}

void Onimaru::Update(bool lockMovement) {
	if (isAlive) {
		Player::Update();
		if (!ultimateInUse && !blastInUse) {
			if (Input::GetMouseButtonRepeat(0) || Input::GetMouseButtonDown(0)) {
				if (!shooting) {
					if (compAnimation) {
						if (!shieldInUse) {
							compAnimation->SendTriggerSecondary(compAnimation->GetCurrentState()->name + states[static_cast<int>(SHOOTING)]);
						}
						else {
							compAnimation->SendTriggerSecondary(compAnimation->GetCurrentState()->name + states[static_cast<int>(SHOOTSHIELD)]);
						}
					}
					shooting = true;
				}
				else {
					Shoot();
				}
			} else if (Input::GetMouseButtonUp(0)) {
				if (shooting) {
					if (compAnimation) {
						compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + compAnimation->GetCurrentState()->name);
					}
					shooting = false;
				}
			}
			if (CanBlast()) {
				if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_Q)) {
					if (shooting) {
						shooting = false;
						if (compAnimation) {
							compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + compAnimation->GetCurrentState()->name);
						}
					}
					if (hudControllerScript) {
						hudControllerScript->SetCooldownRetreival(HUDController::Cooldowns::ONIMARU_SKILL_2);
					}
					blastCooldownRemaining = blastCooldown;
					if (compAnimation) {
						Debug::Log((compAnimation->GetCurrentStateSecondary()->name + states[static_cast<int>(BLAST)]).c_str());
						compAnimation->SendTriggerSecondary(compAnimation->GetCurrentState()->name + states[static_cast<int>(BLAST)]);
					}
					Blast();
				}
			}
		}
		//TODO Ability handling
		//Whenever an ability starts being used, make sure that as well as setting the secondary trigger to whatever, if (shooting was true, it must be turned to false)
		if (blastInUse) {
			Blast();
		}

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

float Onimaru::GetRealBlastCooldown() {
	return 1.f - (blastCooldownRemaining / blastCooldown);
}

void Onimaru::AddEnemy(GameObject* enemy) {
	enemiesInMap.push_back(enemy);
}

void Onimaru::RemoveEnemy(GameObject* enemy) {

	std::vector<GameObject*>::iterator enemyToRemove;
	enemyToRemove = std::find(enemiesInMap.begin(), enemiesInMap.end(), enemy);

	if (enemyToRemove != enemiesInMap.end()) {
		enemiesInMap.erase(enemyToRemove);
	}
}
