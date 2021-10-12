#pragma once

#include "Scripting/Script.h"

class ComponentTransform;
class ResourcePrefab;
class GameObject;

class BarrelSpawner : public Script
{
	GENERATE_BODY(BarrelSpawner);

public:

	void Start() override;
	void Update() override;
	void SpawnBarrels();

public:
	int numberBarrelsRound = 2;
	float secondsBetweenRounds = 0.5f;

	UID barrelBossUID = 0;

private:
	std::vector<float3> positionSpawners;
	ResourcePrefab* barrel = nullptr;
	bool spawn = false;
	int currentBarrel = 0;
	float timerSapwn = 0.0f;
};

