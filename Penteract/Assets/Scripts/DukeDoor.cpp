#include "DukeDoor.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "Components/Physics/ComponentBoxCollider.h"
#include "Components/ComponentObstacle.h"

#include "AIDuke.h"

EXPOSE_MEMBERS(DukeDoor) {
     MEMBER(MemberType::GAME_OBJECT_UID, dukeUID),
     MEMBER(MemberType::GAME_OBJECT_UID, doorObstacleUID)
};

GENERATE_BODY_IMPL(DukeDoor);

void DukeDoor::Start() {
	GameObject* duke = GameplaySystems::GetGameObject(dukeUID);
	if (duke) {
		AIDuke* dukeScript = GET_SCRIPT(duke, AIDuke);
		if (dukeScript) {
			dukeScript->SetReady(false);
		}
	}

	GameObject* obstacle = GameplaySystems::GetGameObject(doorObstacleUID);
	if (obstacle) {
		obstacle->Disable();
	}
}

void DukeDoor::Update() {
	
}

void DukeDoor::OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle) {
	ComponentBoxCollider* collider = GetOwner().GetComponent<ComponentBoxCollider>();
	if (collider) {
		collider->Disable();
	}

	GameObject* obstacle = GameplaySystems::GetGameObject(doorObstacleUID);
	if (obstacle) {
		obstacle->Enable();
	}

	GameObject* duke = GameplaySystems::GetGameObject(dukeUID);
	if (duke) {
		AIDuke* dukeScript = GET_SCRIPT(duke, AIDuke);
		if (dukeScript) {
			dukeScript->SetReady(true);
		}
	}
}