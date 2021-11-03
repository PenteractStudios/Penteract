#include "PlayerController.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "AIMeleeGrunt.h"
#include "RangedAI.h"
#include "HUDController.h"
#include "HUDManager.h"
#include "OnimaruBullet.h"
#include "SwitchParticles.h"
#include "GlobalVariables.h"
#include "AIDuke.h"

#include "Math/Quat.h"
#include "Geometry/Plane.h"
#include "Geometry/Frustum.h"
#include "Geometry/LineSegment.h"
#include "Math/float3x3.h"
#include "Math/float2.h"
#include <algorithm>
#include <string>

#define PI 3.14159
#define AUDIOSOURCE_NULL_MSG "shootAudioSource is NULL"
#define SWITCH_INVULNERABILITY_TIME 1.0f

int PlayerController::currentLevel = 1;

EXPOSE_MEMBERS(PlayerController) {
	MEMBER_SEPARATOR("Player References"),
	MEMBER(MemberType::GAME_OBJECT_UID, fangUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruUID),
	MEMBER(MemberType::GAME_OBJECT_UID, mainNodeUID),
	MEMBER_SEPARATOR("HUD"),
	MEMBER(MemberType::GAME_OBJECT_UID, HUDManagerObjectUID),
	MEMBER_SEPARATOR("Camera Settings"),
	MEMBER(MemberType::GAME_OBJECT_UID, cameraUID),
	MEMBER(MemberType::BOOL, useSmoothCamera),
	MEMBER(MemberType::FLOAT, smoothCameraSpeed),
	MEMBER_SEPARATOR("Fang Stats"),
	MEMBER(MemberType::INT, playerFang.ultimateCooldown),
	MEMBER(MemberType::FLOAT, playerFang.ultimateMovementSpeed),
	MEMBER(MemberType::FLOAT, playerFang.lifePoints),
	MEMBER(MemberType::FLOAT, playerFang.normalMovementSpeed),
	MEMBER(MemberType::FLOAT, playerFang.animationSpeedFactor),
	MEMBER(MemberType::FLOAT, playerFang.damageHit),
	MEMBER(MemberType::FLOAT, playerFang.attackSpeed),
	MEMBER(MemberType::FLOAT, playerFang.dashCooldown),
	MEMBER(MemberType::FLOAT, playerFang.dashSpeed),
	MEMBER(MemberType::FLOAT, playerFang.dashDuration),
	MEMBER(MemberType::FLOAT, playerFang.dashDamage),
	MEMBER(MemberType::FLOAT, playerFang.EMPRadius),
	MEMBER(MemberType::FLOAT, playerFang.EMPCooldown),
	MEMBER(MemberType::FLOAT, playerFang.normalOrientationSpeed),
	MEMBER(MemberType::FLOAT, playerFang.orientationThreshold),
	MEMBER(MemberType::FLOAT, playerFang.decelerationRatio),
	MEMBER(MemberType::FLOAT, playerFang.sprintMovementSpeed),
	MEMBER(MemberType::FLOAT, playerFang.aimTime),
	MEMBER(MemberType::FLOAT, fangRecoveryRate),
	MEMBER_SEPARATOR("Fang Shoot"),
	MEMBER(MemberType::GAME_OBJECT_UID, fangRightBulletUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fangLeftBulletUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fangLeftGunUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fangRightGunUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fangLaserUID),
	MEMBER(MemberType::GAME_OBJECT_UID, playerFang.lookAtPointUID),
	MEMBER_SEPARATOR("Fang Abilities"),
	MEMBER(MemberType::GAME_OBJECT_UID, fangParticleDashUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fangUltimateUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fangUltimateVFXUID),
	MEMBER(MemberType::GAME_OBJECT_UID, EMPUID),
	MEMBER(MemberType::GAME_OBJECT_UID, EMPEffectsUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fangDashDamageUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fangRightFootVFX),
	MEMBER(MemberType::GAME_OBJECT_UID, fangLeftFootVFX),
	MEMBER_SEPARATOR("Onimaru Stats"),
	MEMBER(MemberType::FLOAT, playerOnimaru.lifePoints),
	MEMBER(MemberType::FLOAT, playerOnimaru.normalMovementSpeed),
	MEMBER(MemberType::FLOAT, playerOnimaru.animationSpeedFactor),
	MEMBER(MemberType::FLOAT, playerOnimaru.damageHit),
	MEMBER(MemberType::FLOAT, playerOnimaru.attackSpeed),
	MEMBER(MemberType::FLOAT, playerOnimaru.blastCooldown),
	MEMBER(MemberType::FLOAT, playerOnimaru.blastDistance),
	MEMBER(MemberType::FLOAT, playerOnimaru.blastAngle),
	MEMBER(MemberType::FLOAT, playerOnimaru.blastDelay),
	MEMBER(MemberType::FLOAT, playerOnimaru.normalOrientationSpeed),
	MEMBER(MemberType::FLOAT, playerOnimaru.ultimateAttackSpeed),
	MEMBER(MemberType::FLOAT, playerOnimaru.ultimateTotalTime),
	MEMBER(MemberType::FLOAT, playerOnimaru.ultimateOrientationSpeed),
	MEMBER(MemberType::FLOAT, playerOnimaru.ultimateMovementSpeed),
	MEMBER(MemberType::INT, playerOnimaru.ultimateChargePoints),
	MEMBER(MemberType::INT, playerOnimaru.ultimateChargePointsTotal),
	MEMBER(MemberType::FLOAT, playerOnimaru.orientationThreshold),
	MEMBER(MemberType::FLOAT, playerOnimaru.decelerationRatio),
	MEMBER(MemberType::FLOAT, playerOnimaru.sprintMovementSpeed),
	MEMBER(MemberType::FLOAT, playerOnimaru.aimTime),
	MEMBER(MemberType::FLOAT, onimaruRecoveryRate),
	MEMBER(MemberType::FLOAT, playerOnimaru.offsetWeaponAngle),
	MEMBER(MemberType::FLOAT, playerOnimaru.limitAngle),
	MEMBER_SEPARATOR("Onimaru Shoot"),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruBulletUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruLaserUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruGunUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruRightHandUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruWeaponUID),
	MEMBER(MemberType::GAME_OBJECT_UID, playerOnimaru.lookAtPointUID),

	MEMBER(MemberType::FLOAT, playerOnimaru.cannonGamepadOrientationSpeed),
	MEMBER(MemberType::FLOAT, playerOnimaru.cannonMouseOrientationSpeed),
	MEMBER_SEPARATOR("Onimaru Abilities"),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruShieldUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruBlastEffectsUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruUltimateBulletUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruRightFootVFX),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruLeftFootVFX),
	MEMBER_SEPARATOR("Switch settings"),
	MEMBER(MemberType::FLOAT, switchDelay),
	MEMBER(MemberType::FLOAT, switchCooldown),
	MEMBER(MemberType::FLOAT, switchDamage),
	MEMBER(MemberType::FLOAT, switchSphereRadius),
	MEMBER(MemberType::GAME_OBJECT_UID, switchParticlesUID),
	MEMBER_SEPARATOR("Upgrade Effect"),
	MEMBER(MemberType::GAME_OBJECT_UID, upgradesParticlesUID),
	MEMBER_SEPARATOR("Debug settings"),
	MEMBER(MemberType::BOOL, debugGetHit),

};

GENERATE_BODY_IMPL(PlayerController);

void PlayerController::Start() {
	// Audio Listener
	listener = GetOwner().GetComponent<ComponentAudioListener>();
	transform = GetOwner().GetComponent<ComponentTransform>();
	if (listener) {
		listener->SetDirection(float3(0.0f, 0.0f, -1.0f), float3(0.0f, 1.0f, 0.0f));
		listener->SetPosition(transform->GetGlobalPosition());
	}

	playerFang.Init(fangUID, fangParticleDashUID, fangLeftGunUID, fangRightGunUID, fangRightBulletUID, fangLeftBulletUID, fangLaserUID, cameraUID, HUDManagerObjectUID, fangDashDamageUID, EMPUID, EMPEffectsUID, fangUltimateUID, fangUltimateVFXUID, fangRightFootVFX, fangLeftFootVFX);
	playerOnimaru.Init(onimaruUID, onimaruWeaponUID, onimaruLaserUID, onimaruBulletUID, onimaruGunUID, onimaruRightHandUID, onimaruShieldUID, onimaruUltimateBulletUID, onimaruBlastEffectsUID, cameraUID, HUDManagerObjectUID, onimaruRightFootVFX, onimaruLeftFootVFX);

	GameObject* HUDManagerGO = GameplaySystems::GetGameObject(HUDManagerObjectUID);
	if (HUDManagerGO) {
		hudManagerScript = GET_SCRIPT(HUDManagerGO, HUDManager);
	}

	camera = GameplaySystems::GetGameObject(cameraUID);
	//animation
	if (camera) {
		compCamera = camera->GetComponent<ComponentCamera>();
		if (compCamera) {
			GameplaySystems::SetRenderCamera(compCamera);
		}
		cameraTransform = camera->GetComponent<ComponentTransform>();
	}

	switchEffects = GameplaySystems::GetGameObject(switchParticlesUID);

	GameObject* upgradeEffects = GameplaySystems::GetGameObject(upgradesParticlesUID);
	if (upgradeEffects) {
		upgradeParticles = upgradeEffects->GetComponent<ComponentParticleSystem>();
	}

	//Get audio sources
	int i = 0;

	for (ComponentAudioSource& src : GetOwner().GetComponents<ComponentAudioSource>()) {
		if (i < static_cast<int>(AudioType::TOTAL)) audios[i] = &src;
		i++;
	}

	sCollider = GetOwner().GetComponent<ComponentSphereCollider>();
	if (sCollider) {
		sCollider->radius = switchSphereRadius;
		sCollider->Disable();
	}
	obtainedUpgradeCells = 0;
}

//Debug
void PlayerController::SetInvincible(bool status) {
	invincibleMode = status;
}

void PlayerController::SetOverpower(bool status) {
	overpowerMode = status ? 999 : 0;
}

int PlayerController::GetOverPowerMode() {
	return overpowerMode;
}

float PlayerController::GetOnimaruMaxHealth() const {
	return playerOnimaru.GetTotalLifePoints();
}

float PlayerController::GetFangMaxHealth() const {
	return playerFang.GetTotalLifePoints();
}

bool PlayerController::IsPlayerDead() {
	return !playerFang.isAlive && (!playerOnimaru.isAlive || !GameplaySystems::GetGlobalVariable(globalSwitchTutorialReached, true));
}

void PlayerController::SetNoCooldown(bool status) {
	noCooldownMode = status;
	ResetSwitchStatus();
}
//Switch
bool PlayerController::CanSwitch() {
	return !switchInCooldown && playerOnimaru.CanSwitch() && playerFang.CanSwitch();
}

void PlayerController::ResetSwitchStatus() {
	switchInProgress = false;
	playSwitchParticles = true;
	currentSwitchDelay = 0.f;
}

bool PlayerController::IsVulnerable() const {
	return !invincibleMode && timeSinceLastSwitch >= SWITCH_INVULNERABILITY_TIME;
}

void PlayerController::SwitchCharacter() {
	if (!playerFang.characterGameObject) return;
	if (!playerOnimaru.characterGameObject) return;
	bool doVisualSwitch = currentSwitchDelay < switchDelay ? false : true;

	if (hudManagerScript) {
		hudManagerScript->StartCharacterSwitch();
		hudManagerScript->StartUsingSkill(HUDManager::Cooldowns::SWITCH_SKILL);
	}

	if (sCollider) sCollider->Enable();
	if (doVisualSwitch) {
		if (audios[static_cast<int>(AudioType::SWITCH)]) {
			audios[static_cast<int>(AudioType::SWITCH)]->Play();
		}
		if (playerFang.characterGameObject->IsActive()) {
			playerOnimaru.ResetToIdle();
			playerFang.characterGameObject->Disable();
			playerOnimaru.characterGameObject->Enable();

			fangRecovering = 0.0f;
		} else {
			playerFang.ResetToIdle();
			playerOnimaru.StopAudioOnSwitch();
			playerOnimaru.characterGameObject->Disable();
			playerFang.characterGameObject->Enable();

			onimaruRecovering = 0.0f;
		}

		currentSwitchDelay = 0.f;
		playSwitchParticles = true;
		switchInCooldown = true;
		if (noCooldownMode) switchInProgress = false;
		if (sCollider) sCollider->Disable();
		switchFirstHit = true;

		if (GameplaySystems::GetGlobalVariable(globalswitchTutorialActive, true)) GameplaySystems::SetGlobalVariable(globalswitchTutorialActive, false);

	}
	else {
		if (playSwitchParticles) {
			if (switchEffects) {
				SwitchParticles* script = GET_SCRIPT(switchEffects, SwitchParticles);
				if (script) {
					script->Play();
				}
			}
			switchInProgress = true;
			playSwitchParticles = false;
		}
		if (switchCollisionedGO.size() > 0) {
			for (GameObject* enemy : switchCollisionedGO) {
				AIMeleeGrunt* meleeScript = GET_SCRIPT(enemy, AIMeleeGrunt);
				RangedAI* rangedScript = GET_SCRIPT(enemy, RangedAI);
				AIDuke* dukeScript = GET_SCRIPT(enemy, AIDuke);
				if (rangedScript || meleeScript || dukeScript) {
					if (meleeScript) {
						meleeScript->EnableBlastPushBack();
						if (switchFirstHit) {
							meleeScript->gruntCharacter.GetHit(switchDamage);
							meleeScript->PlayHit();
						}
					}
					else if (rangedScript) {
						rangedScript->EnableBlastPushBack();
						if (switchFirstHit) {
							rangedScript->rangerGruntCharacter.GetHit(switchDamage);
							rangedScript->PlayHit();
						}
					}
					else if (dukeScript) {
						dukeScript->EnableBlastPushBack();
					}
				}
			}
			switchFirstHit = false;
			switchCollisionedGO.clear();
		}
		currentSwitchDelay += Time::GetDeltaTime();
	}
}
//Timers
void PlayerController::CheckCoolDowns() {
	playerFang.CheckCoolDowns(noCooldownMode);
	playerOnimaru.CheckCoolDowns(noCooldownMode);

	if (noCooldownMode || switchCooldownRemaining <= 0.f) {
		switchCooldownRemaining = 0.f;
		switchInCooldown = false;
		if (!noCooldownMode) switchInProgress = false;
	}
	else {
		if (playerOnimaru.characterGameObject->IsActive() && !GameplaySystems::GetGlobalVariable(globalSkill3TutorialReachedOni, true)) switchCooldownRemaining = switchCooldown; // during Onimaru tutorial, do not recover Switch cooldown
		else switchCooldownRemaining -= Time::GetDeltaTime();
	}

	if (playerOnimaru.characterGameObject->IsActive() && playerFang.lifePoints != playerFang.GetTotalLifePoints()) {
		if (fangRecovering >= fangRecoveryRate) {
			fangRecovering = 0.0f;
			playerFang.Recover(1.f);


			if (!playerFang.isAlive) {
				if (playerFang.IsFullHealth()) {
					OnCharacterResurrect();
				}
			}

		}
		else {
			fangRecovering += Time::GetDeltaTime();

		}
	}

	if (playerFang.characterGameObject->IsActive() && playerOnimaru.lifePoints != playerOnimaru.GetTotalLifePoints()) {
		if (onimaruRecovering >= onimaruRecoveryRate) {
			onimaruRecovering = 0.0f;
			playerOnimaru.Recover(1.f);

			if (!playerOnimaru.isAlive) {
				if (playerOnimaru.IsFullHealth()) {
					OnCharacterResurrect();
				}
			}

		}
		else {
			onimaruRecovering += Time::GetDeltaTime();
		}
	}
}
//HUD
void PlayerController::UpdatePlayerStats() {
	float realSwitchCooldown = 1.0f - (switchCooldownRemaining / switchCooldown);

	if (hudManagerScript) {
		if (hitTaken) {
			hudManagerScript->UpdateHealth(playerFang.lifePoints, playerOnimaru.lifePoints);
			hitTaken = false;
		}

		if (playerFang.IsActive() && playerOnimaru.lifePoints <= playerOnimaru.GetTotalLifePoints()) {
			hudManagerScript->HealthRegeneration(playerOnimaru.lifePoints);
		}
		else if (playerOnimaru.IsActive() && playerFang.lifePoints <= playerFang.GetTotalLifePoints()) {
			hudManagerScript->HealthRegeneration(playerFang.lifePoints);
		}

		hudManagerScript->UpdateCooldowns(playerOnimaru.GetRealShieldCooldown(), playerOnimaru.GetRealBlastCooldown(), playerOnimaru.GetRealUltimateCooldown(), playerFang.GetRealDashCooldown(), playerFang.GetRealEMPCooldown(), playerFang.GetRealUltimateCooldown(), realSwitchCooldown, playerFang.ultimateTimeRemaining / playerFang.ultimateTotalTime, playerOnimaru.GetNormalizedRemainingUltimateTime());
	}
}

void PlayerController::TakeDamage(float damage) {
	if (IsVulnerable()) {
		if (playerFang.IsActive()) {
			if (playerFang.IsVulnerable()) {
				playerFang.GetHit(damage);
				hitTaken = true;
			}
		}
		else {
			if (playerOnimaru.IsVulnerable()) {
				playerOnimaru.GetHit(damage);
				hitTaken = true;
			}
		}
	}
}

void PlayerController::AddEnemyInMap(GameObject* enemy) {
	playerOnimaru.AddEnemy(enemy);
}

void PlayerController::RemoveEnemyFromMap(GameObject* enemy) {
	playerOnimaru.RemoveEnemy(enemy);
}

void PlayerController::ObtainUpgradeCell() {
	if (++obtainedUpgradeCells == 3) {
		// TODO: Check whether in level1 or level2 or boss scene
		if (upgradeParticles)upgradeParticles->PlayChildParticles();
		if (currentLevel == 1) Player::level1Upgrade = true;
		else if (currentLevel == 2) Player::level2Upgrade = true;
	}
}

void PlayerController::OnCharacterDeath() {
	SwitchCharacter();

	if (playerFang.isAlive) {
		playerFang.agent->AddAgentToCrowd();
	}
	else {
		playerOnimaru.agent->AddAgentToCrowd();
	}

	if (hudManagerScript) {
		hudManagerScript->OnCharacterDeath();
	}

}

void PlayerController::OnCharacterResurrect() {
	playerOnimaru.isAlive = playerFang.isAlive = true;
	if (hudManagerScript) {
		hudManagerScript->OnCharacterResurrect();
	}
}

void PlayerController::Update() {

	if (GameplaySystems::GetGlobalVariable(globalCameraEventOn, false)) return;

	// Audio Listener
	if (listener) {
		listener->SetPosition(transform->GetGlobalPosition());
	}

	timeSinceLastSwitch = Min(timeSinceLastSwitch + Time::GetDeltaTime(), SWITCH_INVULNERABILITY_TIME);

	if (!playerFang.characterGameObject) return;
	if (!playerOnimaru.characterGameObject) return;
	if (!camera) return;

	if (playerFang.characterGameObject->IsActive()) {
		playerFang.Update(GameplaySystems::GetGlobalVariable<bool>(globalUseGamepad, true));
	}
	else {
		playerOnimaru.Update(GameplaySystems::GetGlobalVariable<bool>(globalUseGamepad, true));
	}

	if (!IsPlayerDead()) {
		CheckCoolDowns();
	}

	UpdatePlayerStats();

	if (CanSwitch()) {

		if (switchInProgress || (noCooldownMode && (Player::GetInputBool(InputActions::SWITCH)))){
			switchInProgress = true;
			SwitchCharacter();
		}

		if (!switchInProgress && (Player::GetInputBool(InputActions::SWITCH))){
			switchInProgress = true;
			timeSinceLastSwitch = 0.0f;
			switchCooldownRemaining = switchCooldown;
		}
	}
}

void PlayerController::OnCollision(GameObject& collidedWith, float3 /* collisionNormal */ , float3 /* penetrationDistance */, void* /* particle */) {
	if (collidedWith.name == "MeleeGrunt" || collidedWith.name == "RangedGrunt" || collidedWith.name == "Duke") {
		switchCollisionedGO.push_back(&collidedWith);
	}
}