#pragma once

class Character
{
public:

	Character() {}
	Character(int lifeSlots_, float speed_, float shootCooldown_)
		: lifeSlots(lifeSlots_)
		, speed(speed_)
		, shootCooldown(shootCooldown_) {
	}

	void Hit(int damage);
	void Accelerate(int acceleration);
	void Decelerate(int deceleration);

public:
	bool isAlive = true;
	bool isEnemy = false;
	bool isBoss = false;

	int lifeSlots = 3;
	float speed = 1.0f;
	float shootCooldown = 0.1f;
};

