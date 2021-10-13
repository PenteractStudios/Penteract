#include "DukeDoor.h"

#include "GameObject.h"
#include "GameplaySystems.h"

#include "Components/Physics/ComponentBoxCollider.h"
#include "Components/ComponentObstacle.h"

EXPOSE_MEMBERS(DukeDoor) {
     MEMBER(MemberType::GAME_OBJECT_UID, dukeUID)
};

GENERATE_BODY_IMPL(DukeDoor);

void DukeDoor::Start() {
	ComponentObstacle* obstacle = GetOwner().GetComponent<ComponentObstacle>();
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

	ComponentObstacle* obstacle = GetOwner().GetComponent<ComponentObstacle>();
	if (obstacle) {
		obstacle->Enable();
	}
}