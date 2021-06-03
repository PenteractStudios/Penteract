#pragma once

#include "Character.h"

class Enemy : public Character {
public:
	// ------- Contructors ------- //
	Enemy() {};

	Enemy(int lifePoints_, float movementSpeed_, int fallingSpeed_, float searchRadius_, float meleeRange_, float timeToDie_)
		: fallingSpeed(fallingSpeed_)
		, searchRadius(searchRadius_) 
		, meleeRange(meleeRange_)
		, timeToDie(timeToDie_) {
		lifePoints = lifePoints_;
		movementSpeed = movementSpeed_;
		totalLifePoints = lifePoints;
	}

	// ------- Core Functions ------ //
	void SetFallingSpeed(float fallingSpeed);
	void SetSearchRadius(float searchRadius);
	void SetMeleeRange(float meleeRange);
	void SetTimeToDie(float timeToDie);

public:
	int fallingSpeed = 5;
	float searchRadius = 10.f;
	float meleeRange = 2.0f;
	float timeToDie = 5.f;
};
