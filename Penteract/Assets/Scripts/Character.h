#pragma once

class Character
{
public:

	Character() {}

	Character(int lifePoints_, float movementSpeed_, float shootCooldown_, float attackSpeed_)
		: lifePoints(lifePoints_)
		, movementSpeed(movementSpeed_)
		, shootCooldown(shootCooldown_)
		, attackSpeed(attackSpeed_){
	}

	void Hit(int damage);
	void Accelerate(int acceleration);
	void Decelerate(int deceleration);

public:
	bool isAlive = true;
	bool isEnemy = false;
	bool isBoss = false;

	int lifePoints = 1;
	float movementSpeed = 1.0f;
	float shootCooldown = 0.1f;
	float attackSpeed = 1.0f;
};
