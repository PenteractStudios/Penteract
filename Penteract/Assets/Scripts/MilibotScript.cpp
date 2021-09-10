#include "MilibotScript.h"
#include "GameObject.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(MilibotScript) {
    // Add members here to expose them to the engine. Example:
    // MEMBER(MemberType::BOOL, exampleMember1),
    // MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
    // MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
    MEMBER(MemberType::FLOAT, rotationSpeed),
    MEMBER(MemberType::FLOAT, positionSpeed),
    MEMBER(MemberType::FLOAT, motionTime),
};

GENERATE_BODY_IMPL(MilibotScript);

void MilibotScript::Start() {
	transform = GetOwner().GetComponent<ComponentTransform>();
}

void MilibotScript::Update() {
    Quat rotation = transform->GetGlobalRotation();
    angle += rotationSpeed * Time::GetDeltaTime();
    Quat newRotation = rotation.RotateAxisAngle(float3(0, 1, 0), angle);
    transform->SetGlobalRotation(newRotation);




    if (restMotionTime > 0) {
        restMotionTime -=  Time::GetDeltaTime();
        newPosition = transform->GetGlobalPosition();
        if (isUp) {
            newPosition.y += positionSpeed * Time::GetDeltaTime();
        }
        else {
            newPosition.y -= positionSpeed * Time::GetDeltaTime();
        }
        transform->SetGlobalPosition(newPosition);
    }
    else {
        restMotionTime = motionTime;
        if (isUp) {
            isUp = false;
        }
        else {
            isUp = true;
        }
    }
}