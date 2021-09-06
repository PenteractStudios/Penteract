#pragma once

#include "Scripting/Script.h"

#include <vector>

class GameObject;
class ResourcePrefab;

class SpawnPointController : public Script {
	GENERATE_BODY(SpawnPointController);

public:
	/* Enemy prefab UIDs */
	UID meleeEnemyPrefabUID = 0;
	UID rangeEnemyPrefabUID = 0;

	/* Door UID */
	UID initialDoorUID = 0;
	UID finalDoorUID = 0;
	UID gameObjectActivatedOnCombatEndUID = 0;

	std::string doorEnergyBack = "DoorEnergyBack";
	std::string doorEnergyFront = "DoorEnergyFront";

	bool unlocksInitialDoor = true;
	float timerToUnlock = 0.0f;

	/* Dissolve UID */
	UID dissolveMaterialGOUID = 0;

public:
	void Start() override;
	void Update() override;

	/* Enable the spawn points on trigger  */
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;
	void OpenDoor();

	ResourcePrefab* GetMeleePrefab() { return meleeEnemyPrefab; };
	ResourcePrefab* GetRangePrefab() { return rangeEnemyPrefab; };

	/* Spawn point status for wave management */
	void SetCurrentEnemyAmount(unsigned int pos, unsigned int amount);
	void SetEnemySpawnStatus(unsigned int pos, bool status);
	bool CanSpawn();

private:
	/* Owner */
	GameObject* gameObject = nullptr;

	/* Enemy prefabs */
	ResourcePrefab* meleeEnemyPrefab = nullptr;
	ResourcePrefab* rangeEnemyPrefab = nullptr;

	/* Door object */
	GameObject* initialDoor = nullptr;
	GameObject* finalDoor = nullptr;

	GameObject* gameObjectActivatedOnCombatEnd = nullptr;	// This gameObject will be disabled when triggering the combat, and will be enabled again when the combat ends (in 'OpenDoor()'). Useful to set up triggers and other gameplay features after a combat encounter.

	/* Spawn points satus*/
	std::vector<unsigned int> enemiesPerSpawnPoint;
	std::vector<bool> enemySpawnPointStatus;

	/* Dissolve UID */
	UID dissolveMaterialID = 0;
	float currentUnlockTime = 0.0f;
	float unlockStarted = false;

private:
	bool CheckSpawnPointStatus();
	void PlayDissolveAnimation(GameObject* root, bool playReverse);
};
