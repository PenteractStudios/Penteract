#pragma once

#include "Scripting/Script.h"

class ComponentTransform;

class AttackDroneProjectile : public Script
{
	GENERATE_BODY(AttackDroneProjectile);

public:

	void Start() override;
	void Update() override;

	void SetDestroyTime(float newDestroyTime);
	void SetSpeed(float newSpeed);

public:
	float speed = 1.0f;
	float destroyTime = 10.0f;

private:
	ComponentTransform* transform = nullptr;

	float currentTime = 0.0f;
};

