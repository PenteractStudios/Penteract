#pragma once

#include "Scripting/Script.h"
#include "Math/float3.h"
#include <vector>

class GameObject;
class ComponentTransform;
class ComponentCamera;
class ComponentAudioSource;
class ComponentParticleSystem;
class HUDController;
class ComponentAgent;
class ComponentAnimation;
class State;
class ResourcePrefab;
struct TesseractEvent;

// We should get these two values from the Character class

#define ONIMARU_MAX_HEALTH 10.0f 
#define FANG_MAX_HEALTH 10.0f

enum class MovementDirection {
	NONE = 0, UP = 1, UP_LEFT = 2, LEFT = 3, DOWN_LEFT = 4, DOWN = 5, DOWN_RIGHT = 6, RIGHT = 7, UP_RIGHT = 8
};

class PlayerController : public Script {
	GENERATE_BODY(PlayerController);

public:

	void Start() override;
	void Update() override;

	void HitDetected();
	bool IsDead();
	void SetInvincible(bool status);
	void SetOverpower(bool status);
	void SetNoCooldown(bool status);

public:
	bool debugGetHit = false;
	GameObject* player = nullptr;
	GameObject* camera = nullptr;
	GameObject* fang = nullptr;
	GameObject* onimaru = nullptr;

	GameObject* fangGun = nullptr;
	GameObject* onimaruGun = nullptr;

	GameObject* onimaruParticle = nullptr;
	ComponentAgent* agent = nullptr;


	UID fangUID = 0;
	UID fangTrailUID = 0;
	UID fangGunUID = 0;

	UID onimaruUID = 0;
	UID onimaruParticleUID = 0;
	UID onimaruTrailUID = 0;
	UID onimaruGunUID = 0;

	UID mainNodeUID = 0;
	UID cameraUID = 0;
	UID canvasUID = 0;

	UID switchAudioSourceUID = 0;
	UID dashAudioSourceUID = 0;

	bool hitTaken = false;
	int lifePointsFang = 10;
	int lifePointsOni = 10;

	float fangAttackSpeed = 1.f;
	float onimaruAttackSpeed = 1.f;
	float distanceRayCast = 2.f;
	float switchCooldown = 5.f;

	float fangMovementSpeed = 10.f;
	float onimaruMovementSpeed = 6.f;

	float dashCooldown = 5.f;
	float dashSpeed = 100.f;
	float dashDuration = 0.1f;

	float cameraOffsetZ = 20.f;
	float cameraOffsetY = 10.f;
	float cameraOffsetX = 0.f;
	float shootCooldown = 0.1f;

	int fangRecoveryRate = 1.0f;
	int onimaruRecoveryRate = 1.0f;

	bool firstTime = true;

	/* Fang & onimaru damage */
	int onimaruDamage = 1;
	int fangDamage = 3;
	
	//Camera
	bool useSmoothCamera = true;
	float smoothCameraSpeed = 5.0f;

	std::vector<std::string> states{ "Idle" ,
								"RunBackward" , "RunForward" , "RunLeft" , "RunRight" ,
								"DashBackward", "DashForward" , "DashLeft" , "DashRight" ,
								"Death" , "Hurt" , "LeftShot" , "RightShot"
	};

private:

	void PlayAnimation(MovementDirection md);
	void MoveTo(MovementDirection md);
	void InitDash(MovementDirection md);
	void Dash();
	void LookAtMouse();
	void CheckCoolDowns();
	void SwitchCharacter();
	void Shoot();
	void UpdatePlayerStats();
	void UpdateCameraPosition();
	bool CanDash();
	bool CanSwitch();
	bool CanShoot();

	float3 GetDirection(MovementDirection md) const;
	MovementDirection GetInputMovementDirection() const;
	int GetMouseDirectionState(MovementDirection input);

private:

	float dashCooldownRemaining = 0.f;
	bool dashInCooldown = false;
	bool dashing = false;
	float dashRemaining = 0.f;

	float switchCooldownRemaining = 0.f;
	bool switchInCooldown = false;

	float fangAttackCooldownRemaining = 0.f;
	float onimaruAttackCooldownRemaining = 0.f;
	bool shooting = false;
	bool invincibleMode = false;
	int overpowerMode = 1;
	bool noCooldownMode = false;

	float fangRecovering = 0.f;
	float onimaruRecovering = 0.f;

	float3 initialPosition = float3(0, 0, 0);
	float3 dashDestination = float3(0, 0, 0);
	float3 dashDirection = float3(0, 0, 0);
	float3 facePointDir = float3(0, 0, 0);
	MovementDirection dashMovementDirection = MovementDirection::NONE;
	ComponentTransform* transform = nullptr;
	ComponentTransform* fangGunTransform = nullptr;
	ComponentTransform* onimaruGunTransform = nullptr;
	ComponentCamera* compCamera = nullptr;
	ComponentTransform* cameraTransform = nullptr;
	//Animation
	ComponentAnimation* fangAnimation = nullptr;
	State* fangCurrentState = nullptr;
	ComponentAnimation* onimaruAnimation = nullptr;
	State* onimaruCurrentState = nullptr;

	ResourcePrefab* fangTrail = nullptr;
	ResourcePrefab* onimaruTrail = nullptr;

	//Particles
	ComponentParticleSystem* fangCompParticle = nullptr;
	ComponentParticleSystem* onimaruCompParticle = nullptr;

	//Audio
	ComponentAudioSource* shootAudioSource = nullptr;
	ComponentAudioSource* dashAudioSource = nullptr;
	ComponentAudioSource* switchAudioSource = nullptr;

	HUDController* hudControllerScript = nullptr;
};
