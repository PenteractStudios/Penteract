#pragma once

#include "Character.h"

class Onimaru : public Character {
public:
	// ------- Contructors ------- //
	Onimaru() {};

	Onimaru(int lifePoints_, float movementSpeed_, int damageHit_, float attackSpeed_)
		:
		attackSpeed(attackSpeed_) {
		lifePoints = lifePoints_;
		movementSpeed = movementSpeed_;
		damageHit = damageHit_;
		SetTotalLifePoints(lifePoints);

	}

public:
	float attackSpeed = 1.0f;
};
