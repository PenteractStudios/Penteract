#include "SpawnPointController.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "EnemySpawnPoint.h"
#include "GameObjectUtils.h"
#include "Components/ComponentLight.h"
#include "FloorIsLava.h"

EXPOSE_MEMBERS(SpawnPointController) {
	MEMBER(MemberType::PREFAB_RESOURCE_UID, meleeEnemyPrefabUID),
		MEMBER(MemberType::PREFAB_RESOURCE_UID, rangeEnemyPrefabUID),
		MEMBER(MemberType::BOOL, unlocksInitialDoor),
		MEMBER(MemberType::BOOL, isInitiallyLocked),
		MEMBER(MemberType::BOOL, isLastDoor),
		MEMBER_SEPARATOR("Activated/Deactivated GameObject Refs"),
		MEMBER(MemberType::GAME_OBJECT_UID, initialDoorUID),
		MEMBER(MemberType::GAME_OBJECT_UID, finalDoorUID),
		MEMBER(MemberType::GAME_OBJECT_UID, gameObjectActivatedOnCombatEndUID),
		MEMBER(MemberType::GAME_OBJECT_UID, gameObjectDeactivatedOnCombatEndUID),
		MEMBER(MemberType::FLOAT, timerToUnlock),
		MEMBER_SEPARATOR("Dissolve material reference in placeholders"),
		MEMBER(MemberType::GAME_OBJECT_UID, dissolveMaterialGOUID),
		MEMBER_SEPARATOR("FireTiles"),
		MEMBER(MemberType::GAME_OBJECT_UID, fireBridgeUID),
		MEMBER(MemberType::GAME_OBJECT_UID, fireArenaUID),
		MEMBER(MemberType::BOOL, stopFire)
};

GENERATE_BODY_IMPL(SpawnPointController);

void SpawnPointController::Start() {
	gameObject = &GetOwner();

	/* Enemy prefabs to be used by the controller's children */
	meleeEnemyPrefab = GameplaySystems::GetResource<ResourcePrefab>(meleeEnemyPrefabUID);
	rangeEnemyPrefab = GameplaySystems::GetResource<ResourcePrefab>(rangeEnemyPrefabUID);

	initialDoor = GameplaySystems::GetGameObject(initialDoorUID);
	if (initialDoor) {
		int i = 0;
		for (ComponentAudioSource& src : initialDoor->GetComponents<ComponentAudioSource>()) {
			if (i < static_cast<int>(Audios::TOTAL)) initialDoorAudios[i] = &src;
			i++;
		}
	}

	finalDoor = GameplaySystems::GetGameObject(finalDoorUID);
	if (finalDoor) {
		int i = 0;
		for (ComponentAudioSource& src : finalDoor->GetComponents<ComponentAudioSource>()) {
			if (i < static_cast<int>(Audios::TOTAL)) finalDoorAudios[i] = &src;
			i++;
		}
	}

	gameObjectActivatedOnCombatEnd = GameplaySystems::GetGameObject(gameObjectActivatedOnCombatEndUID);
	gameObjectDeactivatedOnCombatEnd = GameplaySystems::GetGameObject(gameObjectDeactivatedOnCombatEndUID);

	unsigned int i = 0;
	for (GameObject* child : gameObject->GetChildren()) {
		EnemySpawnPoint* childScript = GET_SCRIPT(child, EnemySpawnPoint);
		if (childScript) childScript->SetIndex(i);
		enemiesPerSpawnPoint.push_back(0);
		enemySpawnPointStatus.push_back(true);
		++i;
	}

	GameObject* dissolveObj = GameplaySystems::GetGameObject(dissolveMaterialGOUID);
	if (dissolveObj) {
		ComponentMeshRenderer* dissolveMeshRenderer = dissolveObj->GetComponent<ComponentMeshRenderer>();
		if (dissolveMeshRenderer) {
			dissolveMaterialID = dissolveMeshRenderer->GetMaterial();
		}
	}

	GameObject* laserDoorLightInitial = SearchReferenceInHierarchy(initialDoor, doorLightGameObjectName);
	if (laserDoorLightInitial) {
		initialDoorLight = laserDoorLightInitial->GetComponent<ComponentLight>();
		if (initialDoorLight) {
			initialDoorLightStartIntensity = initialDoorLight->GetIntensity();
		}
	}

	GameObject* laserDoorLightFinal = SearchReferenceInHierarchy(finalDoor, doorLightGameObjectName);
	if (laserDoorLightFinal) {
		finalDoorLight = laserDoorLightFinal->GetComponent<ComponentLight>();
		if (finalDoorLight) {
			finalDoorLightStartIntensity = finalDoorLight->GetIntensity();
		}
	}

	GameObject* fireBridge = GameplaySystems::GetGameObject(fireBridgeUID);
	if (fireBridge) {
		bridgeTilesScript = GET_SCRIPT(fireBridge, FloorIsLava);
	}
	GameObject* fireArena = GameplaySystems::GetGameObject(fireArenaUID);
	if (fireArena) {
		arenaTilesScript = GET_SCRIPT(fireArena, FloorIsLava);
	}
}

void SpawnPointController::Update() {
	if (unlockStarted) {
		if (currentUnlockTime >= timerToUnlock) {
			if (!isClosing) {	// Must open the door
				if (finalDoor && finalDoor->IsActive()) {
					finalDoor->Disable();
					if (stopFire) {
						if (bridgeTilesScript) bridgeTilesScript->StopFire();
						if (arenaTilesScript) arenaTilesScript->StopFire();
					}
				}
				if (unlocksInitialDoor && initialDoor && initialDoor->IsActive()) {
					initialDoor->Disable();
				}
				gameObject->Disable();
			}
			unlockStarted = false;
			if (!mustKeepOpen) {			// Perform light animation
				SetLightIntensity(initialDoorLight, isClosing ? initialDoorLightStartIntensity : 0.0f);
				SetLightIntensity(finalDoorLight, isClosing ? finalDoorLightStartIntensity : 0.0f);
			}
		} else {
			currentUnlockTime += Time::GetDeltaTime();

			if (!mustKeepOpen) {			// Perform light animation
				float initialDoorNewIntensity = isClosing ? initialDoorLightStartIntensity * (currentUnlockTime / timerToUnlock) : initialDoorLightStartIntensity * (1 - (currentUnlockTime / timerToUnlock));
				float finalDoorNewIntensity = isClosing ? finalDoorLightStartIntensity * (currentUnlockTime / timerToUnlock) : finalDoorLightStartIntensity * (1 - (currentUnlockTime / timerToUnlock));
				SetLightIntensity(initialDoorLight, initialDoorNewIntensity);
				SetLightIntensity(finalDoorLight, finalDoorNewIntensity);
			}
		}
	}
}

void SpawnPointController::OnCollision(GameObject& /* collidedWith */, float3 /* collisionNormal */, float3 /* penetrationDistance */, void* /* particle */) {
	if (!gameObject) return;
	for (GameObject* child : gameObject->GetChildren()) {
		Component* childScript = child->GetComponent<ComponentScript>();
		if (childScript) childScript->Enable();
	}
	if (initialDoor && !initialDoor->IsActive()) initialDoor->Enable();
	if (finalDoor && !finalDoor->IsActive()) finalDoor->Enable();

	if (gameObjectActivatedOnCombatEnd && gameObjectActivatedOnCombatEnd->IsActive()) gameObjectActivatedOnCombatEnd->Disable();
	if (gameObjectDeactivatedOnCombatEnd && !gameObjectDeactivatedOnCombatEnd->IsActive()) gameObjectDeactivatedOnCombatEnd->Enable();

	ComponentBoxCollider* boxCollider = gameObject->GetComponent<ComponentBoxCollider>();
	if (boxCollider) boxCollider->Disable();

	if (!isInitiallyLocked) {
		PlayDissolveAnimation(finalDoor, true);
		PlayDissolveAnimation(initialDoor, true);
		unlockStarted = true;
		isClosing = true;
		ResetUnlockAnimation();
	}
}

void SpawnPointController::OpenDoor() {
	if (CheckSpawnPointStatus()) {
		if (finalDoor && finalDoor->IsActive() && !unlockStarted) {
			PlayDissolveAnimation(finalDoor, false);
		}
		if (unlocksInitialDoor && initialDoor && initialDoor->IsActive() && !unlockStarted) {
			PlayDissolveAnimation(initialDoor, false);
		}

		if (gameObjectActivatedOnCombatEnd && !gameObjectActivatedOnCombatEnd->IsActive()) gameObjectActivatedOnCombatEnd->Enable();
		if (gameObjectDeactivatedOnCombatEnd && gameObjectDeactivatedOnCombatEnd->IsActive()) gameObjectDeactivatedOnCombatEnd->Disable();

		if (!unlockStarted) ResetUnlockAnimation();
		if (!isLastDoor) mustKeepOpen = true;
		unlockStarted = true;
		isClosing = false;
	}
}

void SpawnPointController::SetCurrentEnemyAmount(unsigned int pos, unsigned int amount) {
	enemiesPerSpawnPoint[pos] = amount;
}

void SpawnPointController::SetEnemySpawnStatus(unsigned int pos, bool status) {
	enemySpawnPointStatus[pos] = status;
	if (!status && CheckSpawnPointStatus() && !enemiesSpawned) {
		OpenDoor();
		enemiesSpawned = true;
	}
}

bool SpawnPointController::CanSpawn() {
	return std::all_of(enemiesPerSpawnPoint.begin(), enemiesPerSpawnPoint.end(), [](unsigned int i) { return i == 0; });
}

bool SpawnPointController::CheckSpawnPointStatus() {
	return std::all_of(enemySpawnPointStatus.begin(), enemySpawnPointStatus.end(), [](bool i) { return !i; });
}

void SpawnPointController::PlayDissolveAnimation(GameObject* root, bool playReverse) {
	if (dissolveMaterialID == 0 || !root) return;

	GameObject* doorBack = SearchReferenceInHierarchy(root, doorEnergyBack);
	if (doorBack) {
		ComponentMeshRenderer* meshRenderer = doorBack->GetComponent<ComponentMeshRenderer>();
		if (meshRenderer) {
			meshRenderer->SetMaterial(dissolveMaterialID);
			meshRenderer->PlayDissolveAnimation(playReverse);
		}
	}

	GameObject* doorFront = SearchReferenceInHierarchy(root, doorEnergyFront);
	if (doorFront) {
		ComponentMeshRenderer* meshRenderer = doorFront->GetComponent<ComponentMeshRenderer>();
		if (meshRenderer ) {
			meshRenderer->SetMaterial(dissolveMaterialID);
			meshRenderer->PlayDissolveAnimation(playReverse);
		}
	}

	// Audio Manager
	ComponentAudioSource* down = nullptr;
	ComponentAudioSource* up = nullptr;
	if (root == initialDoor) {
		down = initialDoorAudios[static_cast<int>(Audios::DOWN)];
		up = initialDoorAudios[static_cast<int>(Audios::UP)];
	}
	else if(root == finalDoor) {
		down = finalDoorAudios[static_cast<int>(Audios::DOWN)];
		up = finalDoorAudios[static_cast<int>(Audios::UP)];

	}
	if (playReverse) {
		if (up) up->Play();
	}
	else {
		if (down) down->Play();

	}
}

void SpawnPointController::SetLightIntensity(ComponentLight* light, float newIntensity) {
	if (light) {
		light->SetIntensity(newIntensity);
	}
}

void SpawnPointController::ResetUnlockAnimation() {
	currentUnlockTime = 0.0f;
}