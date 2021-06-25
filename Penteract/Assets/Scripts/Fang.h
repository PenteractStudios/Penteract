#pragma once

#include "Player.h"
class Fang : public Player {
public:


	// ------- Contructors ------- //
	Fang() {};
	void Update(bool lockMovement = false) override;
	void CheckCoolDowns(bool noCooldownMode = false) override;
	float GetRealDashCooldown();
	void Init(int lifePoints_, float movementSpeed_, int damageHit_, float attackSpeed_,UID fangUID, UID trailUID, UID leftGunUID, UID rightGunUID, UID bulletUID, UID cameraUID);

public:
	std::vector<std::string> states{ "Idle" ,
						"RunBackward" , "RunForward" , "RunLeft" , "RunRight" ,
						"DashBackward", "DashForward" , "DashLeft" , "DashRight" ,
						"Death" , "LeftShot" , "RightShot", "RunForwardLeft",
						"RunForwardRight", "RunBackwardLeft", "RunBarckwardRight"
	};
	//pasar a privado y hacer setters
	float dashCooldown = 5.f;
	float dashSpeed = 100.f;
	float dashDuration = 0.1f;


private:
	//Dash
	float dashCooldownRemaining = 0.f;
	float dashRemaining = 0.f;
	bool dashing = false;
	bool dashInCooldown = false;
	float3 initialPosition = float3(0, 0, 0);
	float3 dashDirection = float3(0, 0, 0);


	//Shoot
	bool rightShot = true;
	ComponentTransform* rightGunTransform = nullptr;
	ComponentTransform* leftGunTransform = nullptr;
	ResourcePrefab* trail = nullptr;
	ResourcePrefab* bullet = nullptr;

	//Movement
	MovementDirection dashMovementDirection = MovementDirection::NONE;

private:
	void InitDash();
	void Dash();
	bool CanDash();

	bool CanShoot() override;
	void Shoot() override;
	void PlayAnimation();
};

