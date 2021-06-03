#pragma once

#include "Character.h"

class Player : public Character {
public:
	// ------- Contructors ------- //
	Player() {};

	Player(int lifePoints_, float movementSpeed_, float shootCooldown_, float attackSpeed_)
		: shootCooldown(shootCooldown_)
		, attackSpeed(attackSpeed_) {
		lifePoints = lifePoints_;
		movementSpeed = movementSpeed_;
		totalLifePoints = lifePoints;
	}

	// ------- Core Functions ------ //
	void SetShootCooldown(float shootCooldown);
	void SetAttackSpeed(float attackSpeed);

public:
	float shootCooldown = 0.1f;
	float attackSpeed = 1.0f;
};
