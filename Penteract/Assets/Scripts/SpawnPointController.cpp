#include "SpawnPointController.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "EnemySpawnPoint.h"
#include "GameObjectUtils.h"
#include "Components/ComponentLight.h"
#include "Components/ComponentTransform.h"

#define POS_X_OPEN_INITIAL_BRIDGE 25.f
#define POS_Z_OPEN_INITIAL_BRIDGE 24.f
#define POS_X_CLOSE_INITIAL_BRIDGE 15.f
#define POS_Z_CLOSE_INITIAL_BRIDGE 14.f
#define POS_XZ_OPEN_FINAL_BRIDGE -23.f
#define POS_XZ_CLOSE_FINAL_BRIDGE -13.f

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
	MEMBER_SEPARATOR("Bridge Transport GameObject Refs"),
	MEMBER(MemberType::GAME_OBJECT_UID, initialBridgeUID),
	MEMBER(MemberType::GAME_OBJECT_UID, finalBridgeUID),
	MEMBER(MemberType::GAME_OBJECT_UID, bridgeObstaclesUID),
	MEMBER(MemberType::BOOL, hasToBeEnabledBridges),
	MEMBER(MemberType::BOOL, isTransportArea),
	MEMBER(MemberType::FLOAT, speedAnimationBridges),
	MEMBER_SEPARATOR("Dissolve material reference in placeholders"),
	MEMBER(MemberType::GAME_OBJECT_UID, dissolveMaterialGOUID)
};

GENERATE_BODY_IMPL(SpawnPointController);

void SpawnPointController::Start() {
	gameObject = &GetOwner();

	/* Enemy prefabs to be used by the controller's children */
	meleeEnemyPrefab = GameplaySystems::GetResource<ResourcePrefab>(meleeEnemyPrefabUID);
	rangeEnemyPrefab = GameplaySystems::GetResource<ResourcePrefab>(rangeEnemyPrefabUID);

	initialDoor = GameplaySystems::GetGameObject(initialDoorUID);
	finalDoor = GameplaySystems::GetGameObject(finalDoorUID);

	gameObjectActivatedOnCombatEnd = GameplaySystems::GetGameObject(gameObjectActivatedOnCombatEndUID);
	gameObjectDeactivatedOnCombatEnd = GameplaySystems::GetGameObject(gameObjectDeactivatedOnCombatEndUID);

	initialBridge = GameplaySystems::GetGameObject(initialBridgeUID);
	finalBridge = GameplaySystems::GetGameObject(finalBridgeUID);
	bridgeObstacles = GameplaySystems::GetGameObject(bridgeObstaclesUID);

	if (bridgeObstacles && bridgeObstacles->IsActive()) {
		bridgeObstacles->Disable();
	}
	if (initialBridge && finalBridge) {
		transformInitialBridge = initialBridge->GetComponent<ComponentTransform>();
		transformFinalBridge = finalBridge->GetComponent <ComponentTransform> ();
	}

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
			dissolveMaterialID = dissolveMeshRenderer->materialId;
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
}

void SpawnPointController::Update() {
	if (unlockStarted) {
		if (currentUnlockTime >= timerToUnlock) {
			if (!isClosing) {		
				// Must open the door
				if (finalDoor && finalDoor->IsActive()) {
					finalDoor->Disable();
				}
				if (unlocksInitialDoor && initialDoor && initialDoor->IsActive()) {
					initialDoor->Disable();
				}
				// Must open the bridge
				if (initialBridge && finalBridge) {
					hasToBeEnabledBridges = true;
					isInitiallyLocked = true;
					MoveBridges();
				}
				if (!isTransportArea) {
					gameObject->Disable();
				}
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
	} else {
		// To control the bridges in TRANSPORT
		if (initialBridge && finalBridge) {
			if (!hasToBeEnabledBridges && initialBridge->IsActive() && finalBridge->IsActive()) {
				MoveBridges();
			}
		}
	}
	// To unlock Transport
	if (initialBridge && finalBridge && isInitiallyLocked) {
		if (hasToBeEnabledBridges && initialBridge->IsActive() && finalBridge->IsActive()) {
			MoveBridges();
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
	if (initialBridge && finalBridge && initialBridge->IsActive() && finalBridge->IsActive()) {
		hasToBeEnabledBridges = false;
		// Enabled the obstacles
		if (bridgeObstacles && !bridgeObstacles->IsActive()) {
			bridgeObstacles->Enable();
		}
	}
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

void SpawnPointController::OpenBridges()
{
	// Enable gameobjects
	initialBridge->Enable();
	finalBridge->Enable();
}

void SpawnPointController::CloseBridges()
{
	// Disabled gameobject
	initialBridge->Disable();
	finalBridge->Disable();
}

void SpawnPointController::MoveBridges() {
	if (transformInitialBridge && transformFinalBridge) {
		if (!hasToBeEnabledBridges) {
			if (transformInitialBridge->GetPosition().x <= POS_X_CLOSE_INITIAL_BRIDGE && transformInitialBridge->GetPosition().z <= POS_Z_CLOSE_INITIAL_BRIDGE
				&& transformFinalBridge->GetPosition().x >= POS_XZ_CLOSE_FINAL_BRIDGE && transformFinalBridge->GetPosition().z >= POS_XZ_CLOSE_FINAL_BRIDGE) {
				CloseBridges();
			} else {
				transformInitialBridge->SetPosition(float3(transformInitialBridge->GetPosition().x - (speedAnimationBridges * 1), transformInitialBridge->GetPosition().y, transformInitialBridge->GetPosition().z - (speedAnimationBridges * 1)));
				transformFinalBridge->SetPosition(float3(transformFinalBridge->GetPosition().x + (speedAnimationBridges * 1), transformFinalBridge->GetPosition().y, transformFinalBridge->GetPosition().z + (speedAnimationBridges * 1)));
			}
		}
		else {
			if (isInitiallyLocked) {
				if (!initialBridge->IsActive() && !finalBridge->IsActive()) {
					OpenBridges();
				}
				if (transformInitialBridge->GetPosition().x >= POS_X_OPEN_INITIAL_BRIDGE && transformInitialBridge->GetPosition().z >= POS_Z_OPEN_INITIAL_BRIDGE
					&& transformFinalBridge->GetPosition().x <= POS_XZ_OPEN_FINAL_BRIDGE && transformFinalBridge->GetPosition().z <= POS_XZ_OPEN_FINAL_BRIDGE) {
					// Disabled the obstables
					if (bridgeObstacles && bridgeObstacles->IsActive()) {
						bridgeObstacles->Disable();
					}
					// To put the exact position
					//transformInitialBridge->SetPosition(float3(POS_X_OPEN_INITIAL_BRIDGE, transformInitialBridge->GetPosition().y, POS_Z_OPEN_INITIAL_BRIDGE));
					//transformFinalBridge->SetPosition(float3(POS_XZ_OPEN_FINAL_BRIDGE, transformInitialBridge->GetPosition().y, POS_XZ_OPEN_FINAL_BRIDGE));
					gameObject->Disable();
				} else {
					transformInitialBridge->SetPosition(float3(transformInitialBridge->GetPosition().x + (speedAnimationBridges * 1), transformInitialBridge->GetPosition().y, transformInitialBridge->GetPosition().z + (speedAnimationBridges * 1)));
					transformFinalBridge->SetPosition(float3(transformFinalBridge->GetPosition().x + - (speedAnimationBridges * 1), transformFinalBridge->GetPosition().y, transformFinalBridge->GetPosition().z - (speedAnimationBridges * 1)));
				}
			}
		}
	}
}

void SpawnPointController::SetCurrentEnemyAmount(unsigned int pos, unsigned int amount) {
	enemiesPerSpawnPoint[pos] = amount;
}

void SpawnPointController::SetEnemySpawnStatus(unsigned int pos, bool status) {
	enemySpawnPointStatus[pos] = status;
	if (!status) OpenDoor();
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
			meshRenderer->materialId = dissolveMaterialID;
			meshRenderer->PlayDissolveAnimation(playReverse);
		}
	}

	GameObject* doorFront = SearchReferenceInHierarchy(root, doorEnergyFront);
	if (doorFront) {
		ComponentMeshRenderer* meshRenderer = doorFront->GetComponent<ComponentMeshRenderer>();
		if (meshRenderer ) {
			meshRenderer->materialId = dissolveMaterialID;
			meshRenderer->PlayDissolveAnimation(playReverse);
		}
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
