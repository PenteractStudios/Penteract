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

public:
	// ------- Contructors ------- //
	Onimaru() {};
	void Init(UID onimaruUID = 0, UID onimaruBulletUID = 0, UID onimaruGunUID = 0, UID cameraUID = 0, UID canvasUID = 0, float maxSpread = 5.0f);
	void Update(bool lockMovement = false) override;
	void CheckCoolDowns(bool noCooldownMode = false) override;
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

	float maxBulletSpread = 5.0f;
private:

	bool CanShoot() override;
	void Shoot() override;
	void PlayAnimation();

};
