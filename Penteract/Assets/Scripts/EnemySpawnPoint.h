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
	void KillEnemy();

public:
	UID prefabId = 0;
	UID winUID = 0;
	int amountOfEnemies = 0;
	int offset = 1;

private:
	GameObject* gameObject = nullptr;
	GameObject* winCon = nullptr;
	WinLose* wincondition = nullptr;
	ResourcePrefab* prefab = nullptr;
	int iterator = 0;
	int deadEnemies = 0;
};
