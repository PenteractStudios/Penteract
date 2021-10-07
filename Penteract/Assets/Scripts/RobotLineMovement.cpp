#include "RobotLineMovement.h"

#include "GameplaySystems.h"

EXPOSE_MEMBERS(RobotLineMovement) {
    // Add members here to expose them to the engine. Example:
    // MEMBER(MemberType::BOOL, exampleMember1),
    // MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
    // MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
};

GENERATE_BODY_IMPL(RobotLineMovement);

void RobotLineMovement::Start() {
	
}

void RobotLineMovement::Update() {
    if (destroy) return;

    if (!stopped) {

        if (currentTime >= totalTime) {
            currentTime = totalTime;
        }

        robotTransform->SetGlobalPosition(float3::Lerp(initialPos, finalPos, currentTime / totalTime));

        if (totalTime == currentTime) {
            destroy = true;
        }
        else {
            currentTime += Time::GetDeltaTime();
        }
    }
}

void RobotLineMovement::Initialize(const float3& startPos, const float3& dstPos, float _totalTime) {
    initialPos = startPos;
    finalPos = dstPos;
    totalTime = _totalTime;

    robotTransform = GetOwner().GetComponent<ComponentTransform>();

    if (robotTransform) {
        currentTime += Time::GetDeltaTime();
        robotTransform->SetGlobalPosition(float3::Lerp(initialPos, finalPos, currentTime / totalTime));
    }
}

void RobotLineMovement::Restart() {
    stopped = false;
}

void RobotLineMovement::Stop() {
    stopped = true;
}

bool RobotLineMovement::NeedsToBeDestroyed() const {
    return destroy;
}
