#pragma once

#include "Player.h"

class OnimaruBullet;

class Onimaru : public Player {
public:
	std::vector<std::string> states{ "Idle" ,
					"RunBackward" , "RunForward" , "RunLeft" , "RunRight" ,
					"DashBackward", "DashForward" , "DashLeft" , "DashRight" ,
					"Death" , "Shooting", "","",
					"RunForwardLeft","RunForwardRight", "RunBackwardLeft", "RunBackwardRight"
	};

public:
	// ------- Contructors ------- //
	Onimaru() {};
	void Init(UID onimaruUID = 0, UID onimaruBulletUID = 0, UID onimaruGunUID = 0, UID cameraUID = 0, UID canvasUID = 0, float maxSpread = 5.0f);
	void Update(bool lastInputGamepad = false, bool lockMovement = false) override;
	void CheckCoolDowns(bool noCooldownMode = false) override;
	Quat GetSlightRandomSpread(float minValue, float maxValue) const;
private:

	ResourcePrefab* trail = nullptr;
	ResourcePrefab* bullet = nullptr;
	ComponentTransform* gunTransform = nullptr;
	ComponentParticleSystem* compParticle = nullptr;
	float maxBulletSpread = 5.0f;
private:

	bool CanShoot() override;
	void Shoot() override;
	void PlayAnimation();

};
