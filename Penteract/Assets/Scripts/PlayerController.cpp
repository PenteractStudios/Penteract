#include "PlayerController.h"

#include "GameObject.h"
#include "GameplaySystems.h"

#include "AIMeleeGrunt.h"
#include "RangedAI.h"
#include "HUDController.h"
#include "OnimaruBullet.h"
#include "SwitchParticles.h"
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

EXPOSE_MEMBERS(PlayerController) {
	// Add members here to expose them to the engine. Example:
	MEMBER(MemberType::GAME_OBJECT_UID, fangUID),
		MEMBER(MemberType::GAME_OBJECT_UID, onimaruUID),
		MEMBER(MemberType::GAME_OBJECT_UID, mainNodeUID),
		MEMBER(MemberType::GAME_OBJECT_UID, cameraUID),
		MEMBER(MemberType::PREFAB_RESOURCE_UID, fangBulletUID),
		MEMBER(MemberType::PREFAB_RESOURCE_UID, fangTrailUID),
		MEMBER(MemberType::PREFAB_RESOURCE_UID, onimaruBulletUID),
		MEMBER(MemberType::GAME_OBJECT_UID, fangLeftGunUID),
		MEMBER(MemberType::GAME_OBJECT_UID, fangRightGunUID),
		MEMBER(MemberType::GAME_OBJECT_UID, onimaruGunUID),
		MEMBER(MemberType::GAME_OBJECT_UID, switchParticlesUID),
		MEMBER(MemberType::GAME_OBJECT_UID, canvasUID),
		MEMBER(MemberType::FLOAT, switchCooldown),
		MEMBER(MemberType::FLOAT, dashCooldown),
		MEMBER(MemberType::FLOAT, dashSpeed),
		MEMBER(MemberType::FLOAT, dashDuration),
		MEMBER(MemberType::FLOAT, cameraOffsetZ),
		MEMBER(MemberType::FLOAT, cameraOffsetY),
		MEMBER(MemberType::FLOAT, cameraOffsetX),
		MEMBER(MemberType::INT, fangCharacter.lifePoints),
		MEMBER(MemberType::FLOAT, fangCharacter.movementSpeed),
		MEMBER(MemberType::INT, fangCharacter.damageHit),
		MEMBER(MemberType::FLOAT, fangCharacter.attackSpeed),
		MEMBER(MemberType::INT, onimaruCharacter.lifePoints),
		MEMBER(MemberType::FLOAT, onimaruCharacter.movementSpeed),
		MEMBER(MemberType::INT, onimaruCharacter.damageHit),
		MEMBER(MemberType::FLOAT, onimaruCharacter.attackSpeed),
		MEMBER(MemberType::INT, rangedDamageTaken),
		MEMBER(MemberType::INT, meleeDamageTaken),
		MEMBER(MemberType::BOOL, useSmoothCamera),
		MEMBER(MemberType::FLOAT, smoothCameraSpeed),
		MEMBER(MemberType::INT, onimaruRecoveryRate),
		MEMBER(MemberType::INT, fangRecoveryRate),
		MEMBER(MemberType::BOOL, debugGetHit),
		MEMBER(MemberType::FLOAT, switchDelay),
};

GENERATE_BODY_IMPL(PlayerController);

// crear player fang y onimaru crear una instancia de cada uno pasando por parametro los datos que necesite
// sacar script de camera

void PlayerController::Start() {
	playerFang = new Fang(fangCharacter.lifePoints, fangCharacter.movementSpeed, fangCharacter.damageHit, fangCharacter.attackSpeed, fangUID, fangTrailUID, fangLeftGunUID, fangRightGunUID, fangBulletUID);
	playerOnimaru = new Onimaru(onimaruCharacter.lifePoints, onimaruCharacter.movementSpeed, onimaruCharacter.damageHit, onimaruCharacter.attackSpeed, onimaruUID, onimaruBulletUID, onimaruGunUID);

	GameObject* canvasGO = GameplaySystems::GetGameObject(canvasUID);
	if (canvasGO) {
		hudControllerScript = GET_SCRIPT(canvasGO, HUDController);
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

	firstTime = true;
	//Get audio sources
	int i = 0;

	for (ComponentAudioSource& src : GetOwner().GetComponents<ComponentAudioSource>()) {
		if (i < static_cast<int>(AudioType::TOTAL)) audios[i] = &src;
		i++;
	}
}
//Debug
void PlayerController::SetInvincible(bool status) {
	invincibleMode = status;
}

void PlayerController::SetOverpower(bool status) {
	overpowerMode = status ? 999 : 1;
}

int PlayerController::GetOverPowerMode() {
	return overpowerMode;
}

void PlayerController::SetNoCooldown(bool status) {
	noCooldownMode = status;
	ResetSwitchStatus();
}
//Switch
bool PlayerController::CanSwitch() {
	return !switchInCooldown;
}

void PlayerController::ResetSwitchStatus() {
	switchInProgress = false;
	playSwitchParticles = true;
	currentSwitchDelay = 0.f;
}

void PlayerController::SwitchCharacter(bool noCooldownMode) {
	if (!playerFang) return;
	if (!playerOnimaru) return;
	if (CanSwitch()) {
		bool doVisualSwitch = currentSwitchDelay < switchDelay ? false : true;
		if (doVisualSwitch) {
			if (audios[static_cast<int>(AudioType::SWITCH)]) {
				audios[static_cast<int>(AudioType::SWITCH)]->Play();
			}
			if (playerFang->characterGameObject->IsActive()) {
				playerFang->characterGameObject->Disable();
				playerOnimaru->characterGameObject->Enable();


				if (hudControllerScript) {
					hudControllerScript->UpdateHP(static_cast<float>(onimaruCharacter.lifePoints), static_cast<float>(fangCharacter.lifePoints));
					hudControllerScript->ResetHealthRegenerationEffects(static_cast<float>(fangCharacter.lifePoints));
				}

				fangRecovering = 0.0f;
			}
			else {
				playerOnimaru->characterGameObject->Disable();
				playerFang->characterGameObject->Enable();

				if (hudControllerScript) {
					hudControllerScript->UpdateHP(static_cast<float>(fangCharacter.lifePoints), static_cast<float>(onimaruCharacter.lifePoints));
					hudControllerScript->ResetHealthRegenerationEffects(static_cast<float>(onimaruCharacter.lifePoints));
				}

				onimaruRecovering = 0.0f;
			}
			if (hudControllerScript) {
				hudControllerScript->ChangePlayerHUD(static_cast<float>(fangCharacter.lifePoints), static_cast<float>(onimaruCharacter.lifePoints));
				hudControllerScript->ResetCooldownProgressBar();
			}
			currentSwitchDelay = 0.f;
			playSwitchParticles = true;
			switchInCooldown = true;
			if (noCooldownMode) switchInProgress = false;
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
			currentSwitchDelay += Time::GetDeltaTime();
		}
	}
}
//Timers
void PlayerController::CheckCoolDowns(bool noCooldownMode) {

	if (noCooldownMode || switchCooldownRemaining <= 0.f) {
		switchCooldownRemaining = 0.f;
		switchInCooldown = false;
		if (!noCooldownMode) switchInProgress = false;
	}
	else {
		switchCooldownRemaining -= Time::GetDeltaTime();
	}

	if (playerOnimaru->characterGameObject->IsActive() && fangCharacter.lifePoints != FANG_MAX_HEALTH) {
		if (fangRecovering >= fangRecoveryRate) {
			fangCharacter.Recover(1);
			fangRecovering = 0.0f;
		}
		else {
			fangRecovering += Time::GetDeltaTime();
		}
	}

	if (playerFang->characterGameObject->IsActive() && onimaruCharacter.lifePoints != ONIMARU_MAX_HEALTH) {
		if (onimaruRecovering >= onimaruRecoveryRate) {
			onimaruCharacter.Recover(1);
			onimaruRecovering = 0.0f;
		}
		else {
			onimaruRecovering += Time::GetDeltaTime();
		}
	}
}
//HUD
void PlayerController::UpdatePlayerStats() {
	if (hudControllerScript) {
		if (firstTime) {
			hudControllerScript->UpdateHP(static_cast<float>(fangCharacter.lifePoints), static_cast<float>(onimaruCharacter.lifePoints));
			firstTime = false;
		}

		if (hitTaken && playerFang->IsActive() && fangCharacter.lifePoints >= 0) {
			hudControllerScript->UpdateHP(static_cast<float>(fangCharacter.lifePoints), static_cast<float>(onimaruCharacter.lifePoints));
			hitTaken = false;
		}
		else if (hitTaken && playerOnimaru->IsActive() && onimaruCharacter.lifePoints >= 0) {
			hudControllerScript->UpdateHP(static_cast<float>(onimaruCharacter.lifePoints), static_cast<float>(fangCharacter.lifePoints));
			hitTaken = false;
		}

		if (playerFang->IsActive() && onimaruCharacter.lifePoints != ONIMARU_MAX_HEALTH) {
			float healthRecovered = (onimaruRecovering / onimaruRecoveryRate);
			if (hudControllerScript) {
				hudControllerScript->HealthRegeneration(onimaruCharacter.lifePoints, healthRecovered);
			}
		}
		else if (playerOnimaru->IsActive() && fangCharacter.lifePoints != FANG_MAX_HEALTH) {
			float healthRecovered = (fangRecovering / fangRecoveryRate);
			if (hudControllerScript) {
				hudControllerScript->HealthRegeneration(fangCharacter.lifePoints, healthRecovered);
			}
		}

		float realSwitchCooldown = 1.0f - (switchCooldownRemaining / switchCooldown);
		hudControllerScript->UpdateCooldowns(0.0f, 0.0f, 0.0f, playerFang->GetRealDashCooldown(), 0.0f, 0.0f, realSwitchCooldown);
	}
}
// new script camera
void PlayerController::UpdateCameraPosition() {
	float3 playerGlobalPos = playerFang->playerMainTransform->GetGlobalPosition();

	float3 desiredPosition = playerGlobalPos + float3(cameraOffsetX, cameraOffsetY, cameraOffsetZ);
	float3 smoothedPosition = desiredPosition;

	if (useSmoothCamera) {
		smoothedPosition = float3::Lerp(cameraTransform->GetGlobalPosition(), desiredPosition, smoothCameraSpeed * Time::GetDeltaTime());
	}

	cameraTransform->SetGlobalPosition(smoothedPosition);
}

void PlayerController::TakeDamage(bool ranged) {
	int damage = (ranged) ? rangedDamageTaken : meleeDamageTaken;
	if (playerFang && playerFang->IsActive()) {
		playerFang->Hit(damage);
	}else if (playerOnimaru) {
		playerOnimaru->Hit(damage);
	}
}

void PlayerController::Update() {
	if (!playerFang) return;
	if (!playerOnimaru) return;
	if (!camera) return;

	if (playerFang->isAlive && playerOnimaru->isAlive) {
		CheckCoolDowns();
		UpdatePlayerStats();
		if (playerFang->characterGameObject->IsActive()) {
			playerFang->Update();
		}
		else {
			playerOnimaru->Update();
		}

		UpdateCameraPosition();

		if (firstTime) {
			if (playerFang->characterGameObject->IsActive()) {
				hudControllerScript->UpdateHP(static_cast<float>(fangCharacter.lifePoints), static_cast<float>(onimaruCharacter.lifePoints));
			}
			else {
				hudControllerScript->UpdateHP(static_cast<float>(onimaruCharacter.lifePoints), static_cast<float>(fangCharacter.lifePoints));
			}
			firstTime = false;
		}

		if (switchInProgress || (noCooldownMode && Input::GetKeyCodeUp(Input::KEYCODE::KEY_R))) {
			switchInProgress = true;
			SwitchCharacter();
		}

		if (!switchInProgress && Input::GetKeyCodeUp(Input::KEYCODE::KEY_R)) {
			switchInProgress = true;
			switchCooldownRemaining = switchCooldown;
		}
	}
}