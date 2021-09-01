#pragma once

#include "Scripting/Script.h"

class ComponentTransform;

class RobotsAssemblyMovement : public Script
{
	GENERATE_BODY(RobotsAssemblyMovement);

public:

	void Start() override;
	void Update() override;

public:
	bool onwards = true;
	float offset = 50.f;
	float speed = 1.f;

private:
	GameObject* robots = nullptr;
	float3 initialPos = float3(0, 0, 0);
	ComponentTransform* robotsTransform = nullptr;
	float distanceMoved = 0.f;
	float3 direction = float3(0, 0, 0);

};

