#pragma once

#include "Scripting/Script.h"

class ComponentTransform;

class FanRotation : public Script
{
	GENERATE_BODY(FanRotation);

public:

	void Start() override;
	void Update() override;

public:
	float rotationSpeed = 0.1f;
	bool playReverse = false;

private:
	ComponentTransform* transform = nullptr;
	float currentDegree = 0.0f;
};

