#pragma once

#include "Character.h"

class PlayerController;

class Enemy : public Character {
public:
	// ------- Contructors ------- //
	Enemy() {};

	Enemy(float lifePoints_, float movementSpeed_, float damageHit_, float fallingSpeed_, float searchRadius_, float attackRange_, float timeToDie_, float pushBackDistance_, float pushBackSpeed_, float slowedDownSpeed_, float slowedDownTime_)
		: fallingSpeed(fallingSpeed_)
		, searchRadius(searchRadius_)
		, attackRange(attackRange_)
		, timeToDie(timeToDie_)
		, pushBackDistance(pushBackDistance_)
		, pushBackTime(pushBackSpeed_)
		, slowedDownSpeed(slowedDownSpeed_)
		, slowedDownTime(slowedDownTime_) {
		lifePoints = lifePoints_;
		movementSpeed = movementSpeed_;
		damageHit = damageHit_;
		SetTotalLifePoints(lifePoints);
	}

	// ------- Core Functions ------ //
	void SetFallingSpeed(float fallingSpeed_);
	void SetSearchRadius(float searchRadius_);
	void SetAttackRange(float attackRange_);
	void SetTimeToDie(float timeToDie_);
	void IncreasePlayerUltimateCharges(PlayerController* player);

public:
	float fallingSpeed = 5.0f;
	float searchRadius = 10.f;
	float attackRange = 2.0f;
	float timeToDie = 5.f;
	float pushBackDistance = 15.f;
	float pushBackTime = 0.2f;
	float slowedDownSpeed = 3.f;
	float slowedDownTime = 2.f;
	bool destroying = false;
	bool beingPushed = false;
	bool slowedDown = false;
	//Barrel
	float barrelDamageTaken = 2.f;
};
