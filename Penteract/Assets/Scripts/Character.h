#pragma once

#include "GameplaySystems.h"

#define MAX_ACCELERATION 9999

class ComponentAudioSource;
class ComponentAgent;
class ComponentAnimation;
class ComponentTransform;

class Character
{
public:
	Character() {}

	virtual void Hit(int damage_);
	void Recover(int recoveryLife_);

	void SetTotalLifePoints(int totalLifePoints_);
	void SetDamageHit(int damageHit_);

public:
	bool isAlive = true;
	int lifePoints = 1;
	int damageHit = 1;
	float movementSpeed = 1.0f;
	ComponentAgent* agent = nullptr;
	ComponentAnimation* compAnimation = nullptr;
	ComponentTransform* characterTransform = nullptr;
	State* currentState = nullptr;
private:
	int totalLifePoints = 1;
	
};
