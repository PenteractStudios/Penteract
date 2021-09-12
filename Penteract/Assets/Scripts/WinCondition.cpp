#include "WinCondition.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "PlayerController.h"

EXPOSE_MEMBERS(WinCondition) {
	MEMBER(MemberType::SCENE_RESOURCE_UID, sceneUID)
};

GENERATE_BODY_IMPL(WinCondition);

void WinCondition::Start() {
	gameObject = &GetOwner();
}

void WinCondition::Update() {}

void WinCondition::OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle) {
	ComponentBoxCollider* boxCollider = gameObject->GetComponent<ComponentBoxCollider>();
	if (boxCollider) boxCollider->Disable();

	if (sceneUID) SceneManager::ChangeScene(sceneUID);
	PlayerController::currentLevel = 2;
}