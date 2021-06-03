#pragma once

class Character
{
public:
	Character() {}

	void Hit(int damage);
	void Recover(int recoveryLife);
	void Accelerate(int acceleration);
	void Decelerate(int deceleration);

	void SetTotalLifePoints(int totalLifePoints);

public:
	bool isAlive = true;

	int lifePoints = 1;
	float movementSpeed = 1.0f;

private:
	int totalLifePoints = 1;
};
