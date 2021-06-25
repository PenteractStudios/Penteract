#pragma once

#include "Player.h"

class OnimaruBullet;

class Onimaru : public Player {
public:
	std::vector<std::string> states{ "Idle" ,
					"RunBackward" , "RunForward" , "RunLeft" , "RunRight" ,
					"DashBackward", "DashForward" , "DashLeft" , "DashRight" ,
					"Death" , "LeftShot" , "RightShot", "RunForwardLeft",
					"RunForwardRight", "RunBackwardLeft", "RunBarckwardRight"
	};

public:
	// ------- Contructors ------- //
	Onimaru() {};
	void Init(int lifePoints_, float movementSpeed_, int damageHit_, float attackSpeed_, UID onimaruUID = 0, UID onimaruBulletUID = 0, UID onimaruGunUID = 0, UID cameraUID = 0);
	void Update(bool lockMovement = false) override;
	void CheckCoolDowns(bool noCooldownMode = false) override;
	
private:

	ResourcePrefab* trail = nullptr;
	ResourcePrefab* bullet = nullptr;
	ComponentTransform* gunTransform = nullptr;
	ComponentParticleSystem* compParticle = nullptr;

private:

	bool CanShoot() override;
	void Shoot() override;
	void PlayAnimation();

};
