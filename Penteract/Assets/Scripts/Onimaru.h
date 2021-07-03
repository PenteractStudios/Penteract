#pragma once

#include "Player.h"

class OnimaruBullet;

class Onimaru : public Player {
public:

	enum ONIMARU_STATES {
		IDLE,
		RUN_BACKWARD,
		RUN_FORWARD,
		RUN_LEFT,
		RUN_RIGHT,
		BLAST,
		NONE1,
		ULTI_INTRO,
		ULTI_LOOP,
		DEATH,
		SHOOTING,
		SHIELD,
		SHOOTSHIELD,
		RUNFORWARDLEFT,
		RUNFORWARDRIGHT,
		RUNBACKWARDLEFT,
		RUNBACKWARDRIGHT
	};

	std::vector<std::string> states{ "Idle" ,
					"RunBackward" , "RunForward" , "RunLeft" , "RunRight" ,
					"EnergyBlast", "" , "UltiIntro" , "UltiLoop" ,
					"Death" , "Shooting", "Shield","ShootingShield" ,
					"RunForwardLeft","RunForwardRight", "RunBackwardLeft", "RunBarckwardRight"
	};

	//Onimaru ultimate related
	float originalAttackSpeed = 0.0f;
	float ultimateAttackSpeed = 20.0f;
	float ultimateTimeRemaining = 0.0f;
	float ultimateTotalTime = 3.0f;
	float ultimateRotationSpeed = 2.0f;
	int ultimateChargePoints = 0;
	int ultimateChargePointsTotal = 10;
	float maxBulletSpread = 5.0f;

public:
	// ------- Contructors ------- //
	Onimaru() {};
	void Init(UID onimaruUID = 0, UID onimaruBulletUID = 0, UID onimaruGunUID = 0, UID cameraUID = 0, UID canvasUID = 0);

	void Update(bool lockMovement = false, bool lockOrientation = false) override;
	void CheckCoolDowns(bool noCooldownMode = false) override;
	void IncreaseUltimateCounter();
	bool CanSwitch() const override;

	void OnAnimationFinished() override;
	void OnDeath() override;
	Quat GetSlightRandomSpread(float minValue, float maxValue) const;
private:

	ResourcePrefab* trail = nullptr;
	ResourcePrefab* bullet = nullptr;
	ComponentTransform* gunTransform = nullptr;
	ComponentParticleSystem* compParticle = nullptr;




	bool shieldInUse = false;
	bool blastInUse = false;
	bool ultimateInUse = false;

private:

	bool CanShoot() override;
	void Shoot() override;
	void PlayAnimation();
	void StartUltimate();
	void FinishUltimate();
};
