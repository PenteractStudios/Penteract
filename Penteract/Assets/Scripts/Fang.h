#pragma once

#include "Player.h"

enum class AudioFang {
	DASH,
	EMP,
	ULTIMATE,
	SWITCH,
	SHOOT,
	HIT,
	DEATH,
	TOTAL
};

class Fang : public Player {
public:


	// ------- Contructors ------- //
	Fang() {};

	Fang(int lifePoints_, float movementSpeed_, int damageHit_, float attackSpeed_)
	{
		attackSpeed = attackSpeed_;
		lifePoints = lifePoints_;
		movementSpeed = movementSpeed_;
		damageHit = damageHit_;
		SetTotalLifePoints(lifePoints);
	}

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

	ComponentAudioSource* audiosFang[static_cast<int>(AudioFang::TOTAL)] = { nullptr };

private:
	//Dash
	float dashCooldownRemaining = 0.f;
	float dashRemaining = 0.f;
	bool dashing = false;
	bool dashInCooldown = false;
	float3 initialPosition = float3(0, 0, 0);
	//fang
	float3 dashDirection = float3(0, 0, 0);


	//Shoot
	bool rightShot = true;
	GameObject* leftGun = nullptr;
	GameObject* rightGun = nullptr;
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

