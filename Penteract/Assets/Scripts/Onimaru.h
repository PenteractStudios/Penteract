#pragma once

#include "Player.h"

class Onimaru : public Player {
public:
	// ------- Contructors ------- //
	Onimaru() {};

	Onimaru(int lifePoints_, float movementSpeed_, int damageHit_, float attackSpeed_)
	{
		attackSpeed = attackSpeed_;
		lifePoints = lifePoints_;
		movementSpeed = movementSpeed_;
		damageHit = damageHit_;
		SetTotalLifePoints(lifePoints);
	}

public:
	

};
