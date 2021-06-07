#include "EnemySpawnPoint.h"

#include "Resources/ResourcePrefab.h"
#include "GameplaySystems.h"
#include "GameObject.h"
#include "WinLose.h"

EXPOSE_MEMBERS(EnemySpawnPoint) {
	MEMBER(MemberType::GAME_OBJECT_UID, winUID),
	MEMBER(MemberType::GAME_OBJECT_UID, nextWaveUID),
	MEMBER(MemberType::INT, amountOfEnemies),
	MEMBER(MemberType::INT, offset),
	MEMBER(MemberType::PREFAB_RESOURCE_UID, prefabUID),
};

GENERATE_BODY_IMPL(EnemySpawnPoint);

void EnemySpawnPoint::Start() {
	gameObject = &GetOwner();
	prefab = GameplaySystems::GetResource<ResourcePrefab>(prefabId);
	winCon = GameplaySystems::GetGameObject(winUID);
	if (winCon) winConditionScript = GET_SCRIPT(winCon, WinLose);
	if (nextWaveUID) nextWave = GameplaySystems::GetGameObject(nextWaveUID);
}

void EnemySpawnPoint::Update() {
	for (; iterator < amountOfEnemies; ++iterator) {
		if (prefab != nullptr) {
			UID prefabUID = prefab->BuildPrefab(gameObject);
			GameObject* go = GameplaySystems::GetGameObject(prefabUID);
			ComponentTransform* goTransform = go->GetComponent<ComponentTransform>();
			ComponentBoundingBox* goBounds = nullptr;
			for (auto& child : go->GetChildren()) {
				if (child->HasComponent<ComponentMeshRenderer>()) {
					goBounds = child->GetComponent<ComponentBoundingBox>();
					break;
				}
			}
			if (goTransform && goBounds) {
				float3 newPosition = float3(0, 0, 0);
				float newXval = goBounds->GetLocalMaxPointAABB().x - abs(goBounds->GetLocalMinPointAABB().x);
				newXval = newXval < 1.f ? 1.f : newXval;
				newPosition.x += iterator * offset * newXval;
				goTransform->SetPosition(newPosition);
			}
		}
	}
	if (nextWave && !gameObject->HasChildren()) {
		nextWave->Enable();
	}
}