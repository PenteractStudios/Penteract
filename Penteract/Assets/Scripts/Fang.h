#pragma once

#include "Player.h"

class Fang : public Player {
public:


	// ------- Contructors ------- //
	Fang() {};
	Fang(int lifePoints_, float movementSpeed_, int damageHit_, float attackSpeed_, UID fangUID = 0, UID fangTrailUID = 0, UID fangLeftGunUID = 0, UID fangRightGunUID = 0,
		UID fangBulletUID = 0);
	void Update(MovementDirection md);
	void CheckCoolDowns(bool noCooldownMode = false);

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
	GameObject * fang = nullptr;
	ComponentTransform* rightGunTransform = nullptr;
	ComponentTransform* leftGunTransform = nullptr;
	ResourcePrefab* trail = nullptr;
	ResourcePrefab* bullet = nullptr;

	//Movement
	MovementDirection dashMovementDirection = MovementDirection::NONE;

private:
	void InitDash(MovementDirection md);
	void Dash();
	bool CanDash();

	bool CanShoot() override;
	void Shoot() override;

};

