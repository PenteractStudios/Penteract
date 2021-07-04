#pragma once

#include "Player.h"

class OnimaruBullet;
class HUDController;

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

	// Blast ability
	float blastCooldown = 7.f;
	float blastDistance = 15.f;
	float blastAngle = 50.f;

public:
	// ------- Contructors ------- //
	Onimaru() {};
	void Init(UID onimaruUID = 0, UID onimaruBulletUID = 0, UID onimaruGunUID = 0, UID onimaruRightHand = 0, UID cameraUID = 0, UID canvasUID = 0, float maxSpread = 5.0f);
	void Update(bool lockMovement = false) override;
	void CheckCoolDowns(bool noCooldownMode = false) override;
	void OnDeath() override;
	void OnAnimationFinished() override;
	void OnAnimationSecondaryFinished() override;
	Quat GetSlightRandomSpread(float minValue, float maxValue) const;


	// Abilities' cooldowns
	float GetRealBlastCooldown();

	/* Update enemies' vector */
	void AddEnemy(GameObject* enemy);
	void RemoveEnemy(GameObject* enemy);
private:

	ResourcePrefab* trail = nullptr;
	ResourcePrefab* bullet = nullptr;
	ComponentTransform* gunTransform = nullptr;
	ComponentTransform* rightHand = nullptr;
	ComponentParticleSystem* compParticle = nullptr;

	bool shieldInUse = false;
	bool blastInUse = false;
	bool ultimateInUse = false;

	float maxBulletSpread = 5.0f;

	// Blast ability
	float blastDuration = 1.5f;
	float currentBlastDuration = 0.f;
	float blastCooldownRemaining = 0.f;
	float blastRemaining = 0.f;
	bool blastInCooldown = false;

	// HUD
	HUDController* hudControllerScript = nullptr;


	std::vector<GameObject*> enemiesInMap;
private:

	bool CanShoot() override;
	bool CanBlast();
	void Shoot() override;
	void Blast();
	void PlayAnimation();

};
