#include "SpawnPointController.h"

#include "GameObject.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(SpawnPointController) {};

GENERATE_BODY_IMPL(SpawnPointController);

void SpawnPointController::Start() {
	gameObject = &GetOwner();
}

void SpawnPointController::Update() {}

void SpawnPointController::OnCollision(GameObject& collidedWith) {
	for (GameObject* child : gameObject->GetChildren()) {
		if (!child->IsActive()) child->Enable();
	}
	ComponentBoxCollider* boxCollider = gameObject->GetComponent<ComponentBoxCollider>();
	if (boxCollider) boxCollider->Disable();
}