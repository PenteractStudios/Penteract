#pragma once

#include "Scripting/Script.h"

class GameObject;
class ResourcePrefab;
class WinLose;

class EnemySpawnPoint : public Script {
	GENERATE_BODY(EnemySpawnPoint);

public:
	void Start() override;
	void Update() override;

public:
	UID winUID = 0;
	UID prefabUID = 0;
	UID nextWaveUID = 0;
	int amountOfEnemies = 0;
	int offset = 1;

private:
	GameObject* gameObject = nullptr;
	GameObject* winCon = nullptr;
	WinLose* winConditionScript = nullptr;
	GameObject* nextWave = nullptr;
	ResourcePrefab* prefab = nullptr;
	int iterator = 0;
};
