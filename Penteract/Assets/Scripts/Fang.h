#pragma once

#include "Player.h"

class HUDController;
class Fang : public Player {
public:


	// ------- Contructors ------- //
	Fang() {};
	void Update(bool lockMovement = false) override;
	void CheckCoolDowns(bool noCooldownMode = false) override;
	float GetRealDashCooldown();
	void Init(UID fangUID = 0, UID trailUID = 0, UID leftGunUID = 0, UID rightGunUID = 0, UID bulletUID = 0, UID cameraUID = 0, UID canvasUID = 0);
	void GetHit(float damage_) override;

public:
	std::vector<std::string> states{ "Idle" ,
						"RunBackward" , "RunForward" , "RunLeft" , "RunRight" , //1 - 4
						"DashBackward", "DashForward" , "DashLeft" , "DashRight" , //5 - 8 
						"Death" , "LeftShot" , "RightShot", "" //9 - 12 
						"RunForwardLeft", "RunForwardRight", "RunBackwardLeft", "RunBarckwardRight" // 13 - 16
	};

	bool rightShot = true;

	//Dash
	float dashCooldown = 5.f;
	float dashSpeed = 100.f;
	float dashDuration = 0.1f;

private:
	float dashCooldownRemaining = 0.f;
	float dashRemaining = 0.f;
	bool dashing = false;
	bool dashInCooldown = false;
	float3 initialPosition = float3(0, 0, 0);
	float3 dashDirection = float3(0, 0, 0);

	//Shoot
	ComponentTransform* rightGunTransform = nullptr;
	ComponentTransform* leftGunTransform = nullptr;
	ResourcePrefab* trail = nullptr;
	ResourcePrefab* bullet = nullptr;

	//Movement
	MovementDirection dashMovementDirection = MovementDirection::NONE;

	//HUD
	HUDController* hudControllerScript = nullptr;

private:
	void InitDash();
	void Dash();
	bool CanDash();

	bool CanShoot() override;
	void Shoot() override;
	void PlayAnimation();
};

