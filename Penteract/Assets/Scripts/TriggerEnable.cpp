#include "TriggerEnable.h"

#include "GameplaySystems.h"

EXPOSE_MEMBERS(TriggerEnable) {
    // Add members here to expose them to the engine. Example:
    // MEMBER(MemberType::BOOL, exampleMember1),
    // MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
    MEMBER(MemberType::GAME_OBJECT_UID, objectToEnableUID),
    MEMBER(MemberType::GAME_OBJECT_UID, objectToDisableUID),
    MEMBER(MemberType::BOOL, disableTrigger)
};

GENERATE_BODY_IMPL(TriggerEnable);

void TriggerEnable::Start() {
    objectToEnable = GameplaySystems::GetGameObject(objectToEnableUID);
    objectToDisable = GameplaySystems::GetGameObject(objectToDisableUID);
}

void TriggerEnable::Update() {

}

void TriggerEnable::OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle)
{
    if (objectToEnable) objectToEnable->Enable();
    if (objectToDisable) objectToDisable->Disable();
    if (disableTrigger) GetOwner().Disable();
}

