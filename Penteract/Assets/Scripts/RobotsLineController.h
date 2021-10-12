#pragma once

#include "Scripting/Script.h"

class ResourcePrefab;

class RobotsLineController : public Script
{
	GENERATE_BODY(RobotsLineController);

public:
	float timeBetweenSpawns = 0.8f;
	float timeBetweenStops = 1.5f;
	float timeStopped = 0.8f;
	float timeToReachDestination = 15.f;
	bool changeDirection = 1.f;
	float lineLength = 160.f;

	UID prefab = 0;

public:

	void Start() override;
	void Update() override;

private:

	GameObject* robotsParent = nullptr;

	float3 initialPos = { 0,0,0 };
	float3 finalPos = { 0,0,0 };
	float3 direction = { 0,0,0 };

	int forwardReversed = 1;

	ResourcePrefab* robotPrefab = nullptr;

	bool robotsStopped = false;
	bool allRobotsDeployed = false;

	unsigned totalRobotsToDeploy = 0;
	unsigned robotsDeployed = 0;

	unsigned robotToMove = 0;

	/* Timers */

	float spawnNextRobotTimer = 0.f;
	float stopInTimer = 0.f;
	float stoppedTimer = 0.f;

};

