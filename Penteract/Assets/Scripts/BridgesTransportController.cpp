#include "BridgesTransportController.h"

#include "GameObject.h"
#include "GameplaySystems.h"

#define POS_X_OPEN_INITIAL_BRIDGE 25.f
#define POS_Z_OPEN_INITIAL_BRIDGE 24.f
#define POS_X_CLOSE_INITIAL_BRIDGE 15.f
#define POS_Z_CLOSE_INITIAL_BRIDGE 14.f
#define POS_XZ_OPEN_FINAL_BRIDGE -23.f
#define POS_XZ_CLOSE_FINAL_BRIDGE -13.f

EXPOSE_MEMBERS(BridgesTransportController) {
	MEMBER_SEPARATOR("Bridge Transport GameObject Refs"),
	MEMBER(MemberType::GAME_OBJECT_UID, initialBridgeUID),
	MEMBER(MemberType::GAME_OBJECT_UID, finalBridgeUID),
	MEMBER(MemberType::GAME_OBJECT_UID, bridgeObstaclesUID),
	MEMBER(MemberType::BOOL, hasToBeEnabledBridges),
	MEMBER(MemberType::BOOL, isClosedBridges),
	MEMBER(MemberType::FLOAT, speedAnimationBridges),
};

GENERATE_BODY_IMPL(BridgesTransportController);

void BridgesTransportController::Start() {
	gameObject = &GetOwner();

	if (gameObject->HasChildren()) {
		childEncounter = gameObject->GetChild("Encounter_1");
	}

	initialBridge = GameplaySystems::GetGameObject(initialBridgeUID);
	finalBridge = GameplaySystems::GetGameObject(finalBridgeUID);
	bridgeObstacles = GameplaySystems::GetGameObject(bridgeObstaclesUID);

	if (bridgeObstacles && bridgeObstacles->IsActive()) {
		bridgeObstacles->Disable();
	}
	if (initialBridge && finalBridge) {
		transformInitialBridge = initialBridge->GetComponent<ComponentTransform>();
		transformFinalBridge = finalBridge->GetComponent <ComponentTransform>();
	}
}

void BridgesTransportController::Update() {
	if (initialBridge && finalBridge) {
		if (isClosedBridges) {
			if (hasToBeEnabledBridges) {
				MoveBridges();
			} else {
				if (!childEncounter->IsActive()) hasToBeEnabledBridges = true;
			}
		}
		else {
			// To lock Transport
			if (!hasToBeEnabledBridges && initialBridge->IsActive() && finalBridge->IsActive()) {
				MoveBridges();
			}
		}
	}
}


void BridgesTransportController::OnCollision(GameObject&, float3, float3 penetrationDistance, void* particle)
{
	if (initialBridge && finalBridge && initialBridge->IsActive() && finalBridge->IsActive()) {
		hasToBeEnabledBridges = false;
		// Enabled the obstacles
		if (bridgeObstacles && !bridgeObstacles->IsActive()) {
			bridgeObstacles->Enable();
		}
	}
}

void BridgesTransportController::OpenBridges()
{
	// Enable gameobjects
	initialBridge->Enable();
	finalBridge->Enable();
}

void BridgesTransportController::CloseBridges()
{
	// Disabled gameobject
	initialBridge->Disable();
	finalBridge->Disable();

	// Confirm that the bridges are closed
	isClosedBridges = true;
}

void BridgesTransportController::MoveBridges()
{
	if (transformInitialBridge && transformFinalBridge) {
		if (!hasToBeEnabledBridges) {
			if (transformInitialBridge->GetPosition().x <= POS_X_CLOSE_INITIAL_BRIDGE && transformInitialBridge->GetPosition().z <= POS_Z_CLOSE_INITIAL_BRIDGE
				&& transformFinalBridge->GetPosition().x >= POS_XZ_CLOSE_FINAL_BRIDGE && transformFinalBridge->GetPosition().z >= POS_XZ_CLOSE_FINAL_BRIDGE) {
				CloseBridges();
			}
			else {
				transformInitialBridge->SetPosition(float3(transformInitialBridge->GetPosition().x - (speedAnimationBridges * 1), transformInitialBridge->GetPosition().y, transformInitialBridge->GetPosition().z - (speedAnimationBridges * 1)));
				transformFinalBridge->SetPosition(float3(transformFinalBridge->GetPosition().x + (speedAnimationBridges * 1), transformFinalBridge->GetPosition().y, transformFinalBridge->GetPosition().z + (speedAnimationBridges * 1)));
			}
		}
		else {
			if (!initialBridge->IsActive() && !finalBridge->IsActive()) {
				OpenBridges();
			}
			if (transformInitialBridge->GetPosition().x >= POS_X_OPEN_INITIAL_BRIDGE && transformInitialBridge->GetPosition().z >= POS_Z_OPEN_INITIAL_BRIDGE
				&& transformFinalBridge->GetPosition().x <= POS_XZ_OPEN_FINAL_BRIDGE && transformFinalBridge->GetPosition().z <= POS_XZ_OPEN_FINAL_BRIDGE) {
				// Disabled the obstables
				if (bridgeObstacles && bridgeObstacles->IsActive()) {
					bridgeObstacles->Disable();
				}
				isClosedBridges = false;
				gameObject->Disable();
				
			}
			else {
				transformInitialBridge->SetPosition(float3(transformInitialBridge->GetPosition().x + (speedAnimationBridges * 1), transformInitialBridge->GetPosition().y, transformInitialBridge->GetPosition().z + (speedAnimationBridges * 1)));
				transformFinalBridge->SetPosition(float3(transformFinalBridge->GetPosition().x + - (speedAnimationBridges * 1), transformFinalBridge->GetPosition().y, transformFinalBridge->GetPosition().z - (speedAnimationBridges * 1)));
			}
		}
	}
}
