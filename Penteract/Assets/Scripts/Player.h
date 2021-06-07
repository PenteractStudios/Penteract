#pragma once

#include "Character.h"

class Player : public Character {
public:
	// ------- Contructors ------- //
	Player() {};

	Player(int lifePoints_, float movementSpeed_, int damageHit_, float shootCooldown_, float attackSpeed_)
		: shootCooldown(shootCooldown_)
		, attackSpeed(attackSpeed_){
		lifePoints = lifePoints_;
		movementSpeed = movementSpeed_;
		damageHit = damageHit_;
		SetTotalLifePoints(lifePoints);
	}

	// ------- Core Functions ------ //
	void SetShootCooldown(float shootCooldown_);
	void SetAttackSpeed(float attackSpeed_);

public:
	float shootCooldown = 0.1f;
	float attackSpeed = 1.0f;
};
