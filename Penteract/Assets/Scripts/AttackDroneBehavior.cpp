#include "AttackDroneBehavior.h"

#include "GameplaySystems.h"
#include "Components/ComponentTransform.h"

EXPOSE_MEMBERS(AttackDroneBehavior) {
    MEMBER(MemberType::GAME_OBJECT_UID, dronesControllerUID),
    MEMBER(MemberType::FLOAT, droneSpeed),
};

GENERATE_BODY_IMPL(AttackDroneBehavior);

void AttackDroneBehavior::Start() {
    transform = GetOwner().GetComponent<ComponentTransform>();

    dronesController = GameplaySystems::GetGameObject(dronesControllerUID);
    if (dronesController) {
        dronesControllerTransform = dronesController->GetComponent<ComponentTransform>();
    }
}

void AttackDroneBehavior::Update() {
    if (!transform || !dronesControllerTransform) return;

    transform->SetGlobalPosition(float3::Lerp(transform->GetGlobalPosition(), dronesControllerTransform->GetGlobalPosition() + positionOffset, Time::GetDeltaTime() * droneSpeed));
}

void AttackDroneBehavior::Reposition(float3 newPosition) {
    mustReposition = true;
    currentTime = 0.0f;
    currentPosition = newPosition;
}

void AttackDroneBehavior::SetPositionOffset(float3 newOffset) {
    positionOffset = newOffset;
}