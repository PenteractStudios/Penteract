#pragma once

#include "Player.h"

class OnimaruBullet;

class Onimaru : public Player {
public:
	std::vector<std::string> states{ "Idle" ,
					"RunBackward" , "RunForward" , "RunLeft" , "RunRight" ,
					"DashBackward", "DashForward" , "DashLeft" , "DashRight" ,
					"Death" , "Shooting", "",""
					"RunForwardLeft","RunForwardRight", "RunBackwardLeft", "RunBarckwardRight"
	};

public:
	// ------- Contructors ------- //
	Onimaru() {};
	void Init(UID onimaruUID = 0, UID onimaruBulletUID = 0, UID onimaruGunUID = 0, UID cameraUID = 0, UID canvasUID = 0,
		float onimaruUltimateAttackSpeed = 0.0f, float onimaruUltimateTotalTime = 0.0f,
		float onimaruUltimateRotationSpeed = 0.0f, int onimaruUltimateChargePoints = 0, int onimaruUltimateChargePointsTotal = 0);

	void Update(bool lockMovement = false, bool lockOrientation = false) override;
	void CheckCoolDowns(bool noCooldownMode = false) override;
	void IncreaseUltimateCounter();
	bool CanSwitch() const override;

private:

	ResourcePrefab* trail = nullptr;
	ResourcePrefab* bullet = nullptr;
	ComponentTransform* gunTransform = nullptr;
	ComponentParticleSystem* compParticle = nullptr;

	//Onimaru ultimate related
	float originalAttackSpeed = 0.0f;
	float onimaruUltimateAttackSpeed = 20.0f;
	float onimaruUltimateTimeRemaining = 0.0f;
	float onimaruUltimateTotalTime = 3.0f;
	float onimaruUltimateRotationSpeed = 2.0f;
	int onimaruUltimateChargePoints = 0;
	int onimaruUltimateChargePointsTotal = 10;


private:

	bool CanShoot() override;
	void Shoot() override;
	void PlayAnimation();
	void StartUltimate();
	void FinishUltimate();
};
