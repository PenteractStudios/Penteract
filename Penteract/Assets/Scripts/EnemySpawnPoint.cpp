#include "EnemySpawnPoint.h"

#include "Resources/ResourcePrefab.h"
#include "GameplaySystems.h"
#include "GameObject.h"
//#include "WinLose.h"

EXPOSE_MEMBERS(EnemySpawnPoint) {
	MEMBER(MemberType::INT, amountOfEnemies),
	MEMBER(MemberType::INT, offset),
	MEMBER(MemberType::PREFAB_RESOURCE_UID, prefabId),
};

GENERATE_BODY_IMPL(EnemySpawnPoint);

void EnemySpawnPoint::Start() {
	gameObject = &GetOwner();
	prefab = GameplaySystems::GetResource<ResourcePrefab>(prefabId);
}

void EnemySpawnPoint::Update() {
	for (; iterator < amountOfEnemies; ++iterator) {
		if (prefab != nullptr) {
			UID prefabId = prefab->BuildPrefab(gameObject);
			GameObject* go = GameplaySystems::GetGameObject(prefabId);
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
}

/*void EnemySpawnPoint::KillEnemy() {
	deadEnemies++;
	//Debug::Log("dead: %d", deadEnemies);
	GameObject* winlose = GameplaySystems::GetGameObject("WinCon");
	if (winlose != nullptr) {
		WinLose* wincondition = GET_SCRIPT(winlose, WinLose);
		if (wincondition != nullptr) {
			//Debug::Log("ENEMY DEATH TO WINCONDITION");
			wincondition->KillEnemy();
		}
	}
}*/