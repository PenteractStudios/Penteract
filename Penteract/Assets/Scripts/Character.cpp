#include "Character.h"

void Character::Hit(int damage_) {
	lifePoints -= damage_;
	if (lifePoints <= 0) {
		isAlive = false;
	}
}

void Character::Recover(int recoveryLife_)
{
	int difference = totalLifePoints - lifePoints;
	if (difference > recoveryLife_) {
		lifePoints = totalLifePoints;
	} else {
		lifePoints += recoveryLife_;
	}
}

void Character::Accelerate(int acceleration_)
{
	movementSpeed += acceleration_;
}

void Character::Decelerate(int deceleration_)
{
	movementSpeed -= deceleration_;
	if (movementSpeed < 0.0f) {
		movementSpeed = 0.0f;
	}
}

void Character::SetTotalLifePoints(int totalLifePoints_)
{
	totalLifePoints = totalLifePoints_;
}
