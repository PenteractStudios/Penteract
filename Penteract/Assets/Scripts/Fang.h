#pragma once

#include "Player.h"


class HUDController;
class Fang : public Player {
public:

	// ------- Contructors ------- //
	Fang() {};
	void Update(bool lockMovement = false) override;
	void CheckCoolDowns(bool noCooldownMode = false) override;
	void OnAnimationFinished() override;
	void GetHit(float damage_) override;

	float GetRealDashCooldown();
	void Init(UID fangUID = 0, UID trailGunUID = 0, UID trailDashUID = 0, UID leftGunUID = 0, UID rightGunUID = 0, UID rightBulletUID = 0, UID leftBulletUID = 0, UID cameraUID = 0, UID canvasUID = 0);

public:
	std::vector<std::string> states{ "Idle" ,
						"RunBackward" , "RunForward" , "RunLeft" , "RunRight" , //1 - 4
						"DashBackward", "DashForward" , "DashLeft" , "DashRight" , //5 - 8
						"Death" , "LeftShot" , "RightShot", "", //9 - 12
						"RunForwardLeft", "RunForwardRight", "RunBackwardLeft", "RunBackwardRight", // 13 - 16
						"DashBackward", "DashForward" , "DashLeft" , "DashRight"
	};

	bool rightShot = true;

	//Dash
	float dashCooldown = 5.f;
	float dashSpeed = 100.f;
	float dashDuration = 0.1f;
	float trailDashOffsetDuration = 0.2f;

private:
	float dashCooldownRemaining = 0.f;
	float dashRemaining = 0.f;
	float trailDuration = 0.2f;
	bool dashing = false;
	bool dashInCooldown = false;
	bool hasDashed = false;

	float3 initialPosition = float3(0, 0, 0);
	float3 dashDirection = float3(0, 0, 0);

	//Shoot
	ComponentTransform* rightGunTransform = nullptr;
	ComponentTransform* leftGunTransform = nullptr;
	ComponentTrail* trailDash = nullptr;
	ResourcePrefab* trailGun = nullptr;
	ComponentParticleSystem* rightBullet = nullptr;
	ComponentParticleSystem* leftBullet = nullptr;

	//Movement
	MovementDirection dashMovementDirection = MovementDirection::NONE;

	//HUD
	HUDController* hudControllerScript = nullptr;

private:
	void InitDash();
	void Dash();
	void trailDelay();
	bool CanDash();

	bool CanShoot() override;
	void Shoot() override;
	void PlayAnimation();
};
