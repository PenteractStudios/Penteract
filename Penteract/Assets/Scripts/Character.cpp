#include "Character.h"

void Character::GetHit(float damage_) {
	bool wasAlive = lifePoints > 0.0f;
	lifePoints -= damage_;
	isAlive = lifePoints > 0.0f;
	if (!isAlive && wasAlive) {
		OnDeath();
	}
}

void Character::OnDeath() {

}

void Character::Recover(int recoveryLife_) {
	int difference = totalLifePoints - lifePoints;
	if (recoveryLife_ > difference) {
		lifePoints = totalLifePoints;
	} else {
		lifePoints += recoveryLife_;
	}
}

void Character::SetTotalLifePoints(int totalLifePoints_) {
	totalLifePoints = totalLifePoints_;
}

void Character::SetDamageHit(int damageHit_) {
	damageHit = damageHit_;
}
