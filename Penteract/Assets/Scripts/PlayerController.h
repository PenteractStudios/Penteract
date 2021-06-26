#pragma once

#include "Player.h"
#include "Scripting/Script.h"
#include "Math/float3.h"
#include <vector>
#include "Fang.h"
#include "Onimaru.h"

class GameObject;
class ComponentTransform;
class ComponentCamera;
class ComponentAudioSource;
class ComponentParticleSystem;
class HUDController;
class OnimaruBullet;
class ComponentAgent;
class ComponentAnimation;
class State;
class ResourcePrefab;
struct TesseractEvent;

// We should get these two values from the Character class

#define ONIMARU_MAX_HEALTH 10.0f 
#define FANG_MAX_HEALTH 10.0f



class PlayerController : public Script {
	GENERATE_BODY(PlayerController);

public:
	enum class AudioType {
		SWITCH,
		TOTAL
	};

	void Start() override;
	void Update() override;

	//Debug
	void SetInvincible(bool status);
	void SetOverpower(bool status);
	void SetNoCooldown(bool status);
	int GetOverPowerMode();
	bool IsPlayerDead() { return !playerFang.isAlive || !playerOnimaru.isAlive; }
	void TakeDamage(bool ranged = false);
public:
	//Debug
	bool invincibleMode = false;
	int overpowerMode = 1;
	bool noCooldownMode = false;

	bool debugGetHit = false;
	// cada clase el suyo
	GameObject* camera = nullptr;

	Onimaru playerOnimaru = Onimaru();
	Fang playerFang = Fang();

	UID fangUID = 0;
	UID fangTrailUID = 0;
	UID fangLeftGunUID = 0;
	UID fangRightGunUID = 0;
	UID fangBulletUID = 0;

	UID onimaruUID = 0;
	UID onimaruParticleUID = 0;
	UID onimaruBulletUID = 0;
	UID onimaruGunUID = 0;

	UID switchParticlesUID = 0;
	UID mainNodeUID = 0;
	//camera
	UID cameraUID = 0;
	//playercontroller
	UID canvasUID = 0;

	bool rightShot = true;
	bool shooting = false;

	bool hitTaken = false;
	
	//player
	float switchCooldown = 5.f;
	//camera
	float cameraOffsetZ = 20.f;
	float cameraOffsetY = 10.f;
	float cameraOffsetX = 0.f;
	//split 	
	float fangRecoveryRate = 1.0f;
	float onimaruRecoveryRate = 1.0f;
	//playercontroller
	bool firstTime = true;
	//player
	bool switchInProgress = false;
	float switchDelay = 0.37f;

	/* Fang & onimaru damage */
	//split
	float rangedDamageTaken = 1.0f;
	float meleeDamageTaken = 1.0f;
	
	//Camera
	bool useSmoothCamera = true;
	float smoothCameraSpeed = 5.0f;
	// split
	std::vector<std::string> states{ "Idle" ,
								"RunBackward" , "RunForward" , "RunLeft" , "RunRight" ,
								"DashBackward", "DashForward" , "DashLeft" , "DashRight" ,
								"Death" , "Hurt" , "LeftShot" , "RightShot", "Shooting", "RunForwardLeft",
								"RunForwardRight", "RunBackwardLeft", "RunBarckwardRight"
	};

private:
	void CheckCoolDowns();
	void SwitchCharacter();
	void UpdatePlayerStats();
	void UpdateCameraPosition();
	bool CanSwitch();
	void ResetSwitchStatus();
	
private:

	float switchCooldownRemaining = 0.f;
	bool switchInCooldown = false;

	//split
	float fangRecovering = 0.f;
	float onimaruRecovering = 0.f;

	//player
	float currentSwitchDelay = 0.f;
	bool playSwitchParticles = true;

	//player
	float3 initialPosition = float3(0, 0, 0);
	//player
	float3 facePointDir = float3(0, 0, 0);
	//split
	ComponentCamera* compCamera = nullptr;
	ComponentTransform* cameraTransform = nullptr;

	GameObject* switchEffects = nullptr;
	//playercontroller
	HUDController* hudControllerScript = nullptr;
	OnimaruBullet* onimaruBulletcript = nullptr;

	ComponentAudioSource* audios[static_cast<int>(AudioPlayer::TOTAL)] = { nullptr };
};
