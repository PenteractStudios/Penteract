#include "BridgeDoorButton.h"

#include "FactoryDoors.h"
#include "FloorIsLava.h"
#include "GameplaySystems.h"
#include "GameObject.h"


EXPOSE_MEMBERS(BridgeDoorButton) {
	MEMBER(MemberType::GAME_OBJECT_UID, bridgeDoorUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fireBridgeUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fireArenaUID),
	MEMBER(MemberType::GAME_OBJECT_UID, doorObstacleUID),
	MEMBER(MemberType::GAME_OBJECT_UID, laserDoorObstacleUID)
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

}

void BridgeDoorButton::Update() {
	if (laserDoorObstacle && laserDoorObstacle->GetComponent<ComponentMeshRenderer>()->HasDissolveAnimationFinished()) {
		laserDoorObstacle->Disable();
	}
}

void BridgeDoorButton::OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle)
{
	if (doorScript && bridgeTilesScript && arenaTilesScript) {
		doorScript->Open();
		bridgeTilesScript->StartFire();
		arenaTilesScript->StartFire();

		ComponentSphereCollider* sphereCollider = gameObject->GetComponent<ComponentSphereCollider>();
		if (sphereCollider) sphereCollider->Disable();

		if (doorObstacle) {
			doorObstacle->Disable();
		}

		if (laserDoorObstacle) {
			laserDoorObstacle->GetComponent<ComponentMeshRenderer>()->PlayDissolveAnimation();
		}
	}
}
