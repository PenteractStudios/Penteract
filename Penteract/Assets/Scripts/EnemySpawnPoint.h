#pragma once

#include "Scripting/Script.h"
#include <vector>
#include <tuple>

class GameObject;
class ResourcePrefab;
class PlayerController;

class EnemySpawnPoint : public Script {
	GENERATE_BODY(EnemySpawnPoint);

public:
	void Start() override;
	void Update() override;

	void UpdateRemainingEnemies();
	int GetAmountofEnemies() { return amountOfEnemies; };

public:
	/* Wave configuration */
	unsigned int firstWaveMeleeAmount = 0;
	unsigned int firstWaveRangeAmount = 0;

	unsigned int secondWaveMeleeAmount = 0;
	unsigned int secondWaveRangeAmount = 0;

	unsigned int thirdWaveMeleeAmount = 0;
	unsigned int thirdWaveRangeAmount = 0;

	unsigned int fourthWaveMeleeAmount = 0;
	unsigned int fourthWaveRangeAmount = 0;

	unsigned int fifthWaveMeleeAmount = 0;
	unsigned int fifthWaveRangeAmount = 0;

	/* Variables to place the enemies */
	float xAxisPos = 0;
	float zAxisPos = 4;

	/* Player */
	UID playerUID = 0;

private:
	/* Owner */
	GameObject* gameObject = nullptr;

	/* Prefabs */
	ResourcePrefab* meleeEnemyPrefab = nullptr;
	ResourcePrefab* rangeEnemyPrefab = nullptr;

	/* Player Controller */
	PlayerController* playerScript = nullptr;

	/* Enemy vector & iterator */
	std::vector<std::tuple<unsigned int, unsigned int>> enemies;
	std::vector<std::tuple<unsigned int, unsigned int>>::iterator it;

	/* Amount of enemies for the win condition */
	unsigned int amountOfEnemies = 0;

	/* Distance between the enemies */
	unsigned int offset = 2;

	/* Flags to handle when to spawn the waves */
	bool spawn = true;
	unsigned int waveRemainingEnemies = 0;

	/* Enemy types */
	enum class EnemyType {
		MELEE,
		RANGE
	};

private:
	void RenderEnemy(EnemyType type, unsigned int amount);
};
