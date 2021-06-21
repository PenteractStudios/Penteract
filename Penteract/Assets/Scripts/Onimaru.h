#pragma once

#include "Player.h"

class Onimaru : public Player {
public:
	// ------- Contructors ------- //
	Onimaru() {};

	Onimaru(int lifePoints_, float movementSpeed_, int damageHit_, float attackSpeed_, UID onimaruUID, UID onimaruBulletUID, UID onimaruGunUID, UID onimaruTrailUID);

private:
	GameObject* onimaru = nullptr;
	ResourcePrefab* onimaruTrail = nullptr;
	ResourcePrefab* onimaruBullet = nullptr;
	ComponentTransform* onimaruGunTransform = nullptr;
	ComponentParticleSystem* onimaruCompParticle = nullptr;

};
