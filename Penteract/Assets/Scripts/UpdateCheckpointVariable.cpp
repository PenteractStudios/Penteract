#include "UpdateCheckpointVariable.h"

#include "GameplaySystems.h"
#include "GameObject.h"
#include "GlobalVariables.h"

EXPOSE_MEMBERS(UpdateCheckpointVariable) {
    MEMBER(MemberType::INT, newLevel),
    MEMBER(MemberType::INT, newCheckpoint),
};

GENERATE_BODY_IMPL(UpdateCheckpointVariable);

void UpdateCheckpointVariable::Start() {
	gameObjectTrigger = &GetOwner();
}

void UpdateCheckpointVariable::Update() {
	
}

void UpdateCheckpointVariable::OnCollision(GameObject& /*collidedWith*/, float3 /*collisionNormal*/, float3 /*penetrationDistance*/, void* /*particle*/) {
	// If the level you will change is bigger -> You're in the next level that you play
	if (newLevel > GameplaySystems::GetGlobalVariable(globalLevel, 1)) {
		GameplaySystems::SetGlobalVariable(globalLevel, newLevel);
		GameplaySystems::SetGlobalVariable(globalCheckpoint, newCheckpoint);
	}
	if (newCheckpoint > GameplaySystems::GetGlobalVariable(globalCheckpoint, 0)) {
		GameplaySystems::SetGlobalVariable(globalCheckpoint, newCheckpoint);
	}

	// Disabled the trigger
	ComponentBoxCollider* boxCollider = gameObjectTrigger->GetComponent<ComponentBoxCollider>();
	if (boxCollider) boxCollider->Disable();
}
