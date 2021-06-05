#pragma once

class Character
{
public:
	Character() {}

	void Hit(int damage);
	void Recover(int recoveryLife);

	void SetTotalLifePoints(int totalLifePoints);
	void SetDamageHit(int damageHit);

public:
	bool isAlive = true;

	int lifePoints = 1;
	float movementSpeed = 1.0f;
	int damageHit = 1;

private:
	int totalLifePoints = 1;
};
