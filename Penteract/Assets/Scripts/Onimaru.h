#pragma once

#include "Player.h"

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

	Onimaru(int lifePoints_, float movementSpeed_, int damageHit_, float attackSpeed_, UID onimaruUID, UID onimaruBulletUID, UID onimaruGunUID);

	void Update();
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
