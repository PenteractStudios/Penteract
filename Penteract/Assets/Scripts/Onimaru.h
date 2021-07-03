#pragma once

#include "Player.h"

class OnimaruBullet;
class HUDController;

class Onimaru : public Player {
public:
	std::vector<std::string> states{ "Idle" ,
					"RunBackward" , "RunForward" , "RunLeft" , "RunRight" ,
					"DashBackward", "DashForward" , "DashLeft" , "DashRight" ,
					"Death" , "Shooting", "","",
					"RunForwardLeft","RunForwardRight", "RunBackwardLeft", "RunBackwardRight"
	};

	// Blast ability
	float blastCooldown = 7.f;
	float blastDistance = 15.f;

public:
	// ------- Contructors ------- //
	Onimaru() {};
	void Init(UID onimaruUID = 0, UID onimaruBulletUID = 0, UID onimaruGunUID = 0, UID cameraUID = 0, UID canvasUID = 0, float maxSpread = 5.0f);
	void Update(bool lockMovement = false) override;
	void CheckCoolDowns(bool noCooldownMode = false) override;
	Quat GetSlightRandomSpread(float minValue, float maxValue) const;


	// Abilities' cooldowns
	float GetRealBlastCooldown();
private:

	ResourcePrefab* trail = nullptr;
	ResourcePrefab* bullet = nullptr;
	ComponentTransform* gunTransform = nullptr;
	ComponentParticleSystem* compParticle = nullptr;
	float maxBulletSpread = 5.0f;

	// Blast ability
	float blastDuration = 1.5;
	float blastCooldownRemaining = 0.f;
	float blastRemaining = 0.f;
	bool blastInCooldown = false;
	bool blasting = false;

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
