#pragma once

#include "Scripting/Script.h"

class GameObject;
class ResourcePrefab;

class EnemySpawnPoint : public Script {
	GENERATE_BODY(EnemySpawnPoint);

public:
	void Start() override;
	void Update() override;

public:
	UID prefabId = 0;
	int amountOfEnemies = 0;
	int offset = 1;

private:
	GameObject* gameObject = nullptr;
	ResourcePrefab* prefab = nullptr;
	int iterator = 0;
};
