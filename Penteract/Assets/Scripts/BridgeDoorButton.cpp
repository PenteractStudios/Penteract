#include "BridgeDoorButton.h"

#include "FactoryDoors.h"
#include "FloorIsLava.h"
#include "GameplaySystems.h"
#include "GameObject.h"


EXPOSE_MEMBERS(BridgeDoorButton) {
	MEMBER_SEPARATOR("Object References"),
	MEMBER(MemberType::GAME_OBJECT_UID, bridgeDoorUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fireBridgeUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fireArenaUID),
	MEMBER(MemberType::GAME_OBJECT_UID, doorObstacleUID),
	MEMBER(MemberType::GAME_OBJECT_UID, laserDoorObstacleUID),
	MEMBER_SEPARATOR("Timing Controls"),
	MEMBER(MemberType::FLOAT, closeButtonTime),
	MEMBER(MemberType::FLOAT, openDoorTime)
};

GENERATE_BODY_IMPL(BridgeDoorButton);

void BridgeDoorButton::Start() {
	gameObject = &GetOwner();
	GameObject* bridgeDoor = GameplaySystems::GetGameObject(bridgeDoorUID);
	if (bridgeDoor) {
		doorScript = GET_SCRIPT(bridgeDoor, FactoryDoors);
	}
	GameObject* fireBridge = GameplaySystems::GetGameObject(fireBridgeUID);
	if (fireBridge) {
		bridgeTilesScript = GET_SCRIPT(fireBridge, FloorIsLava);
	}
	GameObject* fireArena = GameplaySystems::GetGameObject(fireArenaUID);
	if (fireArena) {
		arenaTilesScript = GET_SCRIPT(fireArena, FloorIsLava);
	}

	doorObstacle = GameplaySystems::GetGameObject(doorObstacleUID);

	laserDoorObstacle = GameplaySystems::GetGameObject(laserDoorObstacleUID);

	openedDoor = false;
	elapsedTime = 0;
	elapsedButtonTime = 0;
	ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
	if (transform) initialPosition = transform->GetGlobalPosition();
}

void BridgeDoorButton::Update() {
	if (!laserDoorObstacle || !doorObstacle || !doorScript || !bridgeTilesScript || !arenaTilesScript) return;

	if (moveButton) {
		// Move the button inward
		if (elapsedButtonTime < closeButtonTime) {
			ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
			if (transform) transform->SetGlobalPosition(float3::Lerp(initialPosition, float3(initialPosition.x + 0.75f, initialPosition.y, initialPosition.z), elapsedButtonTime / closeButtonTime));
			elapsedButtonTime += Time::GetDeltaTime();
		} else {
			moveButton = false;
		}
	}


	// HasDissolveAnimationFinished is the trigger to start the chain of effects
	if (laserDoorObstacle->GetComponent<ComponentMeshRenderer>()->HasDissolveAnimationFinished()) {

		if (!openedDoor) {
			// Disable obstacles first
			laserDoorObstacle->Disable();
			doorObstacle->Disable();

			// Activate overcharge particles and emissive light bulb


			// Then open the door after some time
			if (elapsedTime < openDoorTime) {
				elapsedTime += Time::GetDeltaTime();
			} else {
				doorScript->Open();
				elapsedTime = 0;
				openedDoor = true;
			}
		} else {
			// When the door has been opened, activate the fires
			if (!fireEnabled) {
				fireEnabled = true;
				bridgeTilesScript->StartFire();
				arenaTilesScript->StartFire();
			}
		}
	}
}

void BridgeDoorButton::OnCollision(GameObject& /*collidedWith*/, float3 /*collisionNormal*/, float3 /*penetrationDistance*/, void* /*particle*/)
{
	ComponentSphereCollider* sphereCollider = gameObject->GetComponent<ComponentSphereCollider>();
	if (sphereCollider) sphereCollider->Disable();

	moveButton = true;

	if (laserDoorObstacle) {
		laserDoorObstacle->GetComponent<ComponentMeshRenderer>()->PlayDissolveAnimation();
	}
}
