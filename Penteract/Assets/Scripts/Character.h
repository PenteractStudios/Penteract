#pragma once

#include "GameplaySystems.h"

#define MAX_ACCELERATION 9999

class ComponentAudioSource;
class ComponentAgent;
class ComponentAnimation;
class ComponentTransform;

class Character {
public:
	Character() {}

	virtual void GetHit(float damage_);
	virtual void OnDeath();
	void Recover(float recoveryLife_);

	void SetTotalLifePoints(float totalLifePoints_);
	void SetDamageHit(float damageHit_);

	float GetTotalLifePoints() const;
	bool IsFullHealth()const;

	void CalculatePushBackFinalPos(const float3& enemyPos, const float3& playerPos, float pushBackDistance);
public:

	bool isAlive = true;
	float lifePoints = 1.0f;
	float damageHit = 1.0f;
	float movementSpeed = 1.0f;
	ComponentAgent* agent = nullptr;
	ComponentAnimation* compAnimation = nullptr;
	State* currentState = nullptr;
	GameObject* characterGameObject = nullptr;

	// Push
	float3 pushBackInitialPos = { 0,0,0 };
	float3 pushBackFinalPos = { 0,0,0 };
	float3 pushBackDirection = { 0,0,0 };

private:
	float totalLifePoints = 1;
};
