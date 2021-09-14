#include "BridgeDoorButton.h"

#include "GameplaySystems.h"
#include "GameObject.h"


EXPOSE_MEMBERS(BridgeDoorButton) {
	MEMBER(MemberType::GAME_OBJECT_UID, bridgeDoorUID)
};

GENERATE_BODY_IMPL(BridgeDoorButton);

void BridgeDoorButton::Start() {
	gameObject = &GetOwner();
	GameObject* bridgeDoor = GameplaySystems::GetGameObject(bridgeDoorUID);
	if (bridgeDoor) {
		script = GET_SCRIPT(bridgeDoor, Script);
	}
}

void BridgeDoorButton::Update() {
	
}

void BridgeDoorButton::OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle)
{
	if (script) {
		//script->Open();
		ComponentCapsuleCollider* capsuleCollider = gameObject->GetComponent<ComponentCapsuleCollider>();
		if (capsuleCollider) capsuleCollider->Disable();
	}
}
