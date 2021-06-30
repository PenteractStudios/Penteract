#pragma once

#include "Scripting/Script.h"

class GameObject;
class ResourcePrefab;

class SpawnPointController : public Script {
	GENERATE_BODY(SpawnPointController);

public:
	/* Enemy prefab UIDs */
	UID meleeEnemyPrefabUID = 0;
	UID rangeEnemyPrefabUID = 0;

public:
	void Start() override;
	void Update() override;

	/* Enable the spawn points on trigger  */
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance) override;

	ResourcePrefab* GetMeleePrefab() { return meleeEnemyPrefab; };
	ResourcePrefab* GetRangePrefab() { return rangeEnemyPrefab; };

private:
	/* Owner */
	GameObject* gameObject = nullptr;

	/* Enemy prefabs */
	ResourcePrefab* meleeEnemyPrefab = nullptr;
	ResourcePrefab* rangeEnemyPrefab = nullptr;
};
