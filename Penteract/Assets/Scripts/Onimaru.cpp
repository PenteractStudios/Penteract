#include "Onimaru.h"

#include "GameplaySystems.h"
#include "OnimaruBullet.h"
#include "AIMeleeGrunt.h"
#include "RangedAI.h"
#include "HUDController.h"
#include "Shield.h"
#include "CameraController.h"

bool Onimaru::CanShoot() {
	return !shootingOnCooldown;
}

bool Onimaru::CanBlast() {
	return !blastInCooldown && !IsShielding() && !ultimateInUse;
}

void Onimaru::Shoot() {
	if (!gunTransform || !transformForUltimateProjectileOrigin) return;
	if (CanShoot()) {
		shootingOnCooldown = true;
		attackCooldownRemaining = 1.f / attackSpeed;

		//NullRef on ultimate values
		if (playerAudios[static_cast<int>(AudioPlayer::SPECIAL_SHOOT)] == nullptr) {
			playerAudios[static_cast<int>(AudioPlayer::SPECIAL_SHOOT)] = playerAudios[static_cast<int>(AudioPlayer::SHOOT)];
		}
		if (ultimateBullet == nullptr) {
			ultimateBullet = bullet;
		}

		ComponentAudioSource* audioSourceToPlay = !ultimateInUse ? playerAudios[static_cast<int>(AudioPlayer::SHOOT)] : playerAudios[static_cast<int>(AudioPlayer::SPECIAL_SHOOT)];
		ComponentParticleSystem* bulletToInstantiate = !ultimateInUse ? bullet : ultimateBullet;
		/*float3 projectilePos = ultimateInUse ? transformForUltimateProjectileOrigin->GetGlobalPosition() : gunTransform->GetGlobalPosition();*/

		if (audioSourceToPlay) {
			audioSourceToPlay->Play();
		}

		if (ultimateInUse) {
			if (ultimateParticles) {
				ultimateParticles->Play();
			}
		}

		if (bulletToInstantiate) {
			//GameObject* bulletInstance = GameplaySystems::Instantiate(bulletToInstantiate, projectilePos, Quat(0.0f, 0.0f, 0.0f, 0.0f));
			/*if (bulletInstance) {
				OnimaruBullet* onimaruBulletScript = GET_SCRIPT(bulletInstance, OnimaruBullet);
				if (onimaruBulletScript) {
					onimaruBulletScript->SetOnimaruDirection(GetSlightRandomSpread(0, maxBulletSpread) * gunTransform->GetGlobalRotation());
				}
			}*/
		}
	}
}

void Onimaru::Blast() {
	bool releaseBlast = currentBlastDuration <= blastDuration / 2.0f ? false : true;
	if (releaseBlast && calculateEnemiesInRange) {
		calculateEnemiesInRange = false;
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
	}
	else {
		currentBlastDuration += Time::GetDeltaTime();
		if (currentBlastDuration >= 1.8f) OnAnimationSecondaryFinished(); // Temporary hack
	}
}

void Onimaru::PlayAnimation() {
	if (!compAnimation) return;
	if (ultimateInUse || !isAlive) return; //Ultimate will block out all movement and idle from happening

	if (compAnimation->GetCurrentState()) {
		if (movementInputDirection == MovementDirection::NONE) {
			//Primery state machine idle when alive, without input movement
			if (compAnimation->GetCurrentState()->name != states[static_cast<int>(IDLE)]) {
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

void Onimaru::StartUltimate() {
	if (!compAnimation) return;
	if (compAnimation->GetCurrentState()) {
		if (compAnimation->GetCurrentStateSecondary()) {
			if (compAnimation->GetCurrentStateSecondary()->name != compAnimation->GetCurrentState()->name) {
				compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + compAnimation->GetCurrentState()->name);
			}
		}
		compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[static_cast<int>(ULTI_INTRO)]);
	}

	if (playerAudios[static_cast<int>(AudioPlayer::THIRD_ABILITY)] == nullptr) {
		if (!playerAudios[static_cast<int>(AudioPlayer::THIRD_ABILITY)]->IsPlaying()) {
			playerAudios[static_cast<int>(AudioPlayer::THIRD_ABILITY)]->Play();
		}
	}

	ultimateChargePoints = 0;
	orientationSpeed = ultimateRotationSpeed;
	attackSpeed = ultimateAttackSpeed;
	movementInputDirection = MovementDirection::NONE;
	Player::MoveTo();
	ultimateInUse = true;
}

void Onimaru::FinishUltimate() {
	ultimateTimeRemaining = 0;
	orientationSpeed = -1;
	attackSpeed = originalAttackSpeed;
	ultimateInUse = false;
	compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[static_cast<int>(IDLE)]);
}

bool Onimaru::IsShielding() const {
	return shield->GetIsActive();
}

float Onimaru::GetRealShieldCooldown() {
	if (shield == nullptr || shieldGO == nullptr) return 0.0f;
	float realShieldCooldown = 1.0f;
	float chargesWasted = (float)(shield->max_charges - shield->GetNumCharges()) / (float)shield->max_charges;
	if (shield->GetIsActive()) {
		realShieldCooldown = chargesWasted;
	}
	else if (shield->GetCoolDown() > 0) {
		realShieldCooldown = 1.0f - (shieldCooldownRemaining / (shield->GetCoolDown() / (1.0f - chargesWasted)));
	}

	return realShieldCooldown;
}

void Onimaru::CheckCoolDowns(bool noCooldownMode) {
	//AttackCooldown
	if (attackCooldownRemaining <= 0.f) {
		attackCooldownRemaining = 0.f;
		shootingOnCooldown = false;
	}
	else {
		attackCooldownRemaining -= Time::GetDeltaTime();
	}

	if (noCooldownMode) {
		ultimateChargePoints = ultimateChargePointsTotal;
	}
	//Blast Cooldown
	if (noCooldownMode || blastCooldownRemaining <= 0.f) {
		blastCooldownRemaining = 0.f;
		blastInCooldown = false;
	}
	else {
		if (!blastInUse) blastCooldownRemaining -= Time::GetDeltaTime();
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
	if (compAnimation == nullptr) return;
	if (compAnimation->GetCurrentState()) {
		if (compAnimation->GetCurrentState()->name != states[static_cast<int>(DEATH)]) {
			if (compAnimation->GetCurrentStateSecondary()) {
				compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + compAnimation->GetCurrentState()->name);
			}
			compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[static_cast<int>(DEATH)]);
		}
	}
	ultimateInUse = blastInUse = false;
}

bool Onimaru::CanSwitch() const {
	return ultimateTimeRemaining <= 0 && !ultimateInUse && !IsShielding() && !blastInUse;
}

void Onimaru::OnAnimationSecondaryFinished() {
	if (compAnimation) {
		if (blastInUse) {
			Debug::Log("Finish blast");
			blastInUse = false;
			blastInCooldown = true;
			currentBlastDuration = 0.f;
			calculateEnemiesInRange = true;
			if (compAnimation) {
				if (compAnimation->GetCurrentStateSecondary() && compAnimation->GetCurrentState()) {
					compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + compAnimation->GetCurrentState()->name);
				}
			}
		}
	}
}

void Onimaru::Init(UID onimaruUID, UID onimaruBulletUID, UID onimaruGunUID, UID onimaruRightHandUID, UID shieldUID, UID onimaruTransformForUltimateProjectileOriginUID, UID cameraUID, UID canvasUID, float maxSpread_) {
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
		GameObject* bulletAux = GameplaySystems::GetGameObject(onimaruBulletUID);
		if (bulletAux) {
			bullet = bulletAux->GetComponent<ComponentParticleSystem>();
			if (bullet) {
				bullet->SetParticlesPerSecond(float2(0.0f, 0.0f));
				bullet->Play();
			}
		}
	}

	originalAttackSpeed = attackSpeed;

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
		shieldGO->Disable();
	}

	GameObject* onimaruObjForUltProj = GameplaySystems::GetGameObject(onimaruTransformForUltimateProjectileOriginUID);
	if (onimaruObjForUltProj) {
		transformForUltimateProjectileOrigin = onimaruObjForUltProj->GetComponent<ComponentTransform>();
	}

	if (transformForUltimateProjectileOrigin == nullptr) {
		transformForUltimateProjectileOrigin = gunTransform;
	}

	if (characterGameObject) {
		//Get audio sources
		int i = 0;

		for (ComponentAudioSource& src : characterGameObject->GetComponents<ComponentAudioSource>()) {
			if (i < static_cast<int>(AudioPlayer::TOTAL)) playerAudios[i] = &src;
			i++;
		}
	}

	GameObject* rightHandGO = GameplaySystems::GetGameObject(onimaruRightHandUID);
	if (rightHandGO) rightHand = rightHandGO->GetComponent<ComponentTransform>();
}

void Onimaru::OnAnimationFinished() {
	if (!compAnimation)return;
	if (ultimateInUse) {
		ultimateTimeRemaining = ultimateTotalTime;
		if (compAnimation->GetCurrentState()) {
			compAnimation->SendTrigger(compAnimation->GetCurrentState()->name + states[static_cast<int>(ULTI_LOOP)]);
		}
	}
}

bool Onimaru::CanShield() {
	if (shield == nullptr || shieldGO == nullptr) return false;

	return !shieldInCooldown && !shield->GetIsActive() && !ultimateInUse;
}

bool Onimaru::CanUltimate() {
	return !blastInUse && !IsShielding() && ultimateChargePoints >= ultimateChargePointsTotal;
}

void Onimaru::InitShield() {
	if (shield == nullptr || shieldGO == nullptr) return;

	if (CanShield()) {
		shield->InitShield();
		if (shieldParticles) {
			shieldParticles->Play();
		}

		shieldInCooldown = false;
		if (agent) {
			agent->SetMaxSpeed(movementSpeed / 2);
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
	if (shield == nullptr || shieldGO == nullptr) return;
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

void Onimaru::Update(bool lockMovement, bool lockRotation) {
	if (shield == nullptr || shieldGO == nullptr) return;
	if (isAlive) {
		Player::Update(ultimateInUse, false);

		if (!ultimateInUse) {
			if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_E)) {
				if (CanUltimate()) {
					StartUltimate();
				}
			}
		}
		else {
			//Ultimate execution
			if (ultimateTimeRemaining > 0) {
				ultimateTimeRemaining -= Time::GetDeltaTime();
				Shoot();
				if (ultimateTimeRemaining <= 0) {
					FinishUltimate();
				}
			}
		}

		if (!ultimateInUse && !blastInUse) {
			if (Input::GetMouseButtonDown(2)) {
				InitShield();
			}
			if (shield->GetIsActive()) {
				if (Input::GetMouseButtonUp(2) || shield->GetNumCharges() == shield->max_charges) {
					FadeShield();
				}
			}
			if (Input::GetMouseButtonRepeat(0) || Input::GetMouseButtonDown(0)) {
				//7bullet->Play();
					//bullet->SetParticlesPerSecond(float2(attackSpeed, attackSpeed));
				if (!shooting) {
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
				}
				else {
					Shoot();
				}
				shooting = true;
			}
		}

		if (Input::GetMouseButtonUp(0)) {
			if (compAnimation) {
				if (shield->GetIsActive()) {
					if (compAnimation->GetCurrentState() && compAnimation->GetCurrentStateSecondary()) {
						compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + states[static_cast<int>(SHIELD)]);
					}
				}
				else {
					if (compAnimation->GetCurrentStateSecondary() && compAnimation->GetCurrentState()) {
						compAnimation->SendTriggerSecondary(compAnimation->GetCurrentStateSecondary()->name + compAnimation->GetCurrentState()->name);
					}
				}
				bullet->SetParticlesPerSecond(float2(0.0f, 0.0f));
			}
			shooting = false;
		}
		if (CanBlast()) {
			if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_Q)) {
				blastInUse = true;
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
					if (compAnimation->GetCurrentState()) {
						compAnimation->SendTriggerSecondary(compAnimation->GetCurrentState()->name + states[static_cast<int>(BLAST)]);
					}
				}
				Blast();
			}
		}
	}
	else {
		if (agent) agent->RemoveAgentFromCrowd();
		movementInputDirection = MovementDirection::NONE;
	}

	if (blastInUse) {
		Blast();
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

float Onimaru::GetRealUltimateCooldown() {
	return static_cast<float>(ultimateChargePoints) / static_cast<float>(ultimateChargePointsTotal);
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