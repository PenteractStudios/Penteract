#include "WinLose.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "EnemySpawnPoint.h"

EXPOSE_MEMBERS(WinLose) {
	// Add members here to expose them to the engine. Example:
	MEMBER(MemberType::GAME_OBJECT_UID, winUID),
	MEMBER(MemberType::GAME_OBJECT_UID, playerUID),
	MEMBER(MemberType::GAME_OBJECT_UID, enemiesUID),
	MEMBER(MemberType::FLOAT, LoseOffsetX),
	MEMBER(MemberType::FLOAT, LoseOffsetZ)
};

GENERATE_BODY_IMPL(WinLose);

void WinLose::Start() {
	winCon = GameplaySystems::GetGameObject(winUID);
	player = GameplaySystems::GetGameObject(playerUID);
	enemies = GameplaySystems::GetGameObject(enemiesUID);
	if (enemies->HasChildren()) {
		for (GameObject* spawn : enemies->GetChildren()) {
			if (spawn->IsActive()) {
				EnemySpawnPoint* enemyspawnpoint = GET_SCRIPT(spawn, EnemySpawnPoint);
				if (enemyspawnpoint != nullptr) {
					totalEnemies += enemyspawnpoint->amountOfEnemies;
				}
			}
		}
	}
	//Debug::Log("TOTAL ENEMIES: %d", totalEnemies);
}

void WinLose::Update() {
	if (winCon == nullptr) return;
	if (player == nullptr) return;

	ComponentTransform* playerTransform = player->GetComponent<ComponentTransform>();
	ComponentTransform* winConTransform = winCon->GetComponent<ComponentTransform>();
	if (!playerTransform || !winConTransform) return;

	float3 position = playerTransform->GetGlobalPosition();
	float3 winConPos = winConTransform->GetGlobalPosition();
	if (position.x <= winConPos.x + LoseOffsetX
		&& position.x >= winConPos.x - LoseOffsetX
		&& position.z <= winConPos.z + LoseOffsetZ
		&& position.z >= winConPos.z - LoseOffsetZ
		&& deadEnemies >= totalEnemies) {
		SceneManager::ChangeScene("Assets/Scenes/WinScene.scene");
	}
}

void WinLose::KillEnemy() {
	deadEnemies++;
	//Debug::Log("DEAD ENEMIES: %d", deadEnemies);
}