#include "Character.h"

void Character::Hit(int damage) {
	lifePoints -= damage;
	if (lifePoints <= 0) {
		isAlive = false;
	}
}

void Character::Accelerate(int acceleration)
{
	movementSpeed += acceleration;
}

void Character::Decelerate(int deceleration)
{
	movementSpeed -= deceleration;
	if (movementSpeed < 0.0f) {
		movementSpeed = 0.0f;
	}
}
