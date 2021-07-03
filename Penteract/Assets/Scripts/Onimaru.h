#pragma once

#include "Player.h"

class OnimaruBullet;
class HUDController;
class Shield;
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
	void Init(UID onimaruUID = 0, UID onimaruBulletUID = 0, UID onimaruGunUID = 0, UID cameraUID = 0, UID canvasUID = 0, UID shieldUID = 0);
	void Update(bool lockMovement = false) override;
	void CheckCoolDowns(bool noCooldownMode = false) override;

	float GetRealShieldCooldown();
	bool IsShielding();
	
private:

	ResourcePrefab* trail = nullptr;
	ResourcePrefab* bullet = nullptr;
	ComponentTransform* gunTransform = nullptr;
	ComponentParticleSystem* compParticle = nullptr;

	HUDController* hudControllerScript = nullptr;
	Shield* shield = nullptr;
	GameObject* shieldGO = nullptr;
	
	bool shieldInCooldown = false;
	float shieldCooldownRemaining = 0.f;

private:

	bool CanShoot() override;
	void Shoot() override;
	void PlayAnimation();
	void InitShield();
	void FadeShield();
	bool CanShield();

};
