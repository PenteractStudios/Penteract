#include "UpdateCheckpointVariable.h"

#include "GameplaySystems.h"
#include "GameObject.h"
#include "GlobalVariables.h"

EXPOSE_MEMBERS(UpdateCheckpointVariable) {
    MEMBER(MemberType::INT, actualLevel),
    MEMBER(MemberType::INT, newCheckpoint),
};

GENERATE_BODY_IMPL(UpdateCheckpointVariable);

void UpdateCheckpointVariable::Start() {
	
}

void UpdateCheckpointVariable::Update() {
	
}

void UpdateCheckpointVariable::OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle) {
	if (GameplaySystems::GetGlobalVariable(globalLevel, 0) > actualLevel) {
		GameplaySystems::SetGlobalVariable(globalLevel, actualLevel);
		GameplaySystems::SetGlobalVariable(globalCheckpoint, newCheckpoint);
	}
	if (GameplaySystems::GetGlobalVariable(globalCheckpoint, 0) > newCheckpoint) {
		GameplaySystems::SetGlobalVariable(globalCheckpoint, newCheckpoint);
	}

}
