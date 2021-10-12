#include "SpawnEffectScript.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(SpawnEffectScript) {
    // Add members here to expose them to the engine. Example:
    // MEMBER(MemberType::BOOL, exampleMember1),
    // MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
    // MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
    MEMBER(MemberType::FLOAT, ground)
};

GENERATE_BODY_IMPL(SpawnEffectScript);

void SpawnEffectScript::Start() {
    mainTransform = GetOwner().GetComponent<ComponentTransform>();
}

void SpawnEffectScript::Update() {
    float3 newPosition = float3(mainTransform->GetGlobalPosition().x, ground, mainTransform->GetGlobalPosition().z);
    GetOwner().GetComponent<ComponentTransform>()->SetGlobalPosition(newPosition);
}