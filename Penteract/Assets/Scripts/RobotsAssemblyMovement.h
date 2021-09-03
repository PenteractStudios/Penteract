#pragma once

#include "Scripting/Script.h"

class RobotsAssemblyMovement : public Script
{
	GENERATE_BODY(RobotsAssemblyMovement);

public:

	void Start() override;
	void Update() override;

public:

	UID meleeFront = 0;
	int numOfRobots = 10;
	bool changeDirection = false;
	float distanceBetweenRobots = 50.f;
	float lineLenght = 160.f;
	float speed = 1.f;

private:
	GameObject* robotsLine = nullptr;
	float3 initialPos = float3(0, 0, 0);
	float3 finalPos = float3(0, 0, 0);
	float3 direction = float3(0, 0, 0);
	std::vector<GameObject*> robots;
	int forward = 1;
};

