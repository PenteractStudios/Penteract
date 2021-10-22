#include "Character.h"

void Character::GetHit(float damage_) {
	//We assume that the character is always alive when this method gets called, so no need to check if character was alive before taking lives
	lifePoints -= damage_;
	isAlive = lifePoints > 0.0f;
	if (!isAlive) {
		OnDeath();
	}
}

void Character::OnDeath() {

}

void Character::Recover(float recoveryLife_) {  
	float difference = totalLifePoints - lifePoints;
	if (recoveryLife_ > difference) {
		lifePoints = totalLifePoints;
	} else {
		lifePoints += recoveryLife_;
	}
}

void Character::SetTotalLifePoints(float totalLifePoints_) {
	totalLifePoints = totalLifePoints_;
}

float Character::GetTotalLifePoints() const
{
	return totalLifePoints;
}

void Character::SetDamageHit(float damageHit_) {
	damageHit = damageHit_;
}

bool Character::IsFullHealth() const {
	return lifePoints == totalLifePoints;
}

void Character::CalculatePushBackFinalPos(const float3& enemyPos, const float3& playerPos, float pushBackDistance) {
	pushBackDirection = (enemyPos - playerPos).Normalized();

	bool hitResult = false;
	pushBackInitialPos = enemyPos;
	pushBackFinalPos = enemyPos + pushBackDirection * pushBackDistance;
	float3 resultPos = { 0,0,0 };

	Navigation::Raycast(enemyPos, pushBackFinalPos, hitResult, resultPos);

	if (hitResult) {
		pushBackFinalPos = resultPos - pushBackDirection;
	}

	float pushInitialHeight = -100.f;
	float pushFinalHeight = -100.f;

	Navigation::GetNavMeshHeightInPosition(pushBackInitialPos, pushInitialHeight);
	Navigation::GetNavMeshHeightInPosition(pushBackFinalPos, pushFinalHeight);

	if (pushInitialHeight != pushFinalHeight) {
		pushBackFinalPos.y = enemyPos.y + pushFinalHeight - pushInitialHeight;
	}
}
