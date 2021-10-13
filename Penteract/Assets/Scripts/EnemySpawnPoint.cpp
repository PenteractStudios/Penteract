#include "EnemySpawnPoint.h"

#include "Resources/ResourcePrefab.h"
#include "PlayerController.h"
#include "GameplaySystems.h"
#include "GameObject.h"

EXPOSE_MEMBERS(EnemySpawnPoint) {
	MEMBER(MemberType::GAME_OBJECT_UID, playerUID),
	MEMBER(MemberType::INT, firstWaveMeleeAmount),
	MEMBER(MemberType::INT, firstWaveRangeAmount),
	MEMBER(MemberType::INT, secondWaveMeleeAmount),
	MEMBER(MemberType::INT, secondWaveRangeAmount),
	MEMBER(MemberType::INT, thirdWaveMeleeAmount),
	MEMBER(MemberType::INT, thirdWaveRangeAmount),
	MEMBER(MemberType::INT, fourthWaveMeleeAmount),
	MEMBER(MemberType::INT, fourthWaveRangeAmount),
	MEMBER(MemberType::INT, fifthWaveMeleeAmount),
	MEMBER(MemberType::INT, fifthWaveRangeAmount),
};

GENERATE_BODY_IMPL(EnemySpawnPoint);

void EnemySpawnPoint::Start() {
	/* Owner */
	gameObject = &GetOwner();
	gameObjectTransform = gameObject->GetComponent<ComponentTransform>();

	/* Load the wave config into the enemy vector */
	enemies.emplace_back(firstWaveMeleeAmount, firstWaveRangeAmount);
	enemies.emplace_back(secondWaveMeleeAmount, secondWaveRangeAmount);
	enemies.emplace_back(thirdWaveMeleeAmount, thirdWaveRangeAmount);
	enemies.emplace_back(fourthWaveMeleeAmount, fourthWaveRangeAmount);
	enemies.emplace_back(fifthWaveMeleeAmount, fifthWaveRangeAmount);
	it = enemies.begin();

	for (auto const& [melee, range] : enemies) amountOfEnemies += melee + range;

	spawnPointControllerScript = GET_SCRIPT(gameObject->GetParent(), SpawnPointController);
	if (spawnPointControllerScript) {
		meleeEnemyPrefab = spawnPointControllerScript->GetMeleePrefab();
		rangeEnemyPrefab = spawnPointControllerScript->GetRangePrefab();
		spawnPointControllerScript->SetCurrentEnemyAmount(index, amountOfEnemies);
	}

	player = GameplaySystems::GetGameObject(playerUID);
	if (player) {
		playerScript = GET_SCRIPT(player, PlayerController);
	}

	if (gameObject->HasChildren()) gameObject->GetChild("Arrow")->Disable();
}

void EnemySpawnPoint::Update() {
	if (!gameObject->IsActive() || !meleeEnemyPrefab || !rangeEnemyPrefab || !player) return;

	if (it != enemies.end() && spawn) {
		RenderEnemy(EnemyType::MELEE, std::get<0>(*it));
		RenderEnemy(EnemyType::RANGE, std::get<1>(*it));
		/* Rotate the spawn point to the player location */
		float3 playerDirection = player->GetComponent<ComponentTransform>()->GetGlobalPosition() - gameObjectTransform->GetGlobalPosition();
		playerDirection.y = 0.f;		
		LookAtPlayer(playerDirection);

		/* Stop the spawn until all the wave enemies die */
		spawn = false;
		waveRemainingEnemies = std::get<0>(*it) + std::get<1>(*it);
		spawnPointControllerScript->SetCurrentEnemyAmount(index, waveRemainingEnemies);
	}

	/* Condition to spawn the next wave */
	if (waveRemainingEnemies == 0 && spawnPointControllerScript->CanSpawn()) {
		spawn = true;
		if (it != enemies.end()) {
			it++;
		}
		else {
			spawnPointControllerScript->SetEnemySpawnStatus(index, false);
		}
	}
}

/* Called each time an enemy dies */
void EnemySpawnPoint::UpdateRemainingEnemies() {
	waveRemainingEnemies--;
	spawnPointControllerScript->SetCurrentEnemyAmount(index, waveRemainingEnemies);
}

void EnemySpawnPoint::RenderEnemy(EnemyType type, unsigned int amount) {
	for (unsigned int i = 0; i < amount; ++i) {
		UID prefabUID = type == EnemyType::MELEE ? meleeEnemyPrefab->BuildPrefab(gameObject) : rangeEnemyPrefab->BuildPrefab(gameObject);
		GameObject* go = GameplaySystems::GetGameObject(prefabUID);
		ComponentTransform* goTransform = go->GetComponent<ComponentTransform>();
		if (go) {
			goTransform->SetPosition(EnemyLocation(amount, i, type == EnemyType::MELEE ? 0 : -2));
			if (playerScript) playerScript->AddEnemyInMap(go);
		}
	}
}

float3 EnemySpawnPoint::EnemyLocation(int N, int k, int z) {
	/*
		([N / 2] - k) * d - ((N + 1) % 2) * d / 2
		N : amount of enemies
		k : current enemy
		d : separation distance 
		z : enemy position in the z axis

		kudos to Pol for the formula
	*/
	int d = 2;
	float x = static_cast<float>(((N / 2 - k) * d - ((N + 1) % 2) * d / 2));
	return float3(x, 0.0f, static_cast<float>(z));
}

void EnemySpawnPoint::LookAtPlayer(const float3& direction) {
	Quat newRotation = Quat::LookAt(float3(0, 0, 1), direction.Normalized(), float3(0, 1, 0), float3(0, 1, 0));
	gameObjectTransform->SetGlobalRotation(newRotation);
}