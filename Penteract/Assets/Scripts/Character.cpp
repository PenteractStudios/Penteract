#include "Character.h"

void Character::Hit(int damage) {
	lifeSlots -= damage;
	if (lifeSlots <= 0) {
		isAlive = false;
	}
}

void Character::Accelerate(int acceleration)
{
	speed += acceleration;
}

void Character::Decelerate(int deceleration)
{
	speed -= deceleration;
	if (speed < 0.0f) {
		speed = 0.0f;
	}
}
