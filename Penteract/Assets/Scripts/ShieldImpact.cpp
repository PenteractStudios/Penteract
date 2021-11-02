#include "ShieldImpact.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(ShieldImpact) {
    // Add members here to expose them to the engine. Example:
    // MEMBER(MemberType::BOOL, exampleMember1),
    // MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
    // MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
};

GENERATE_BODY_IMPL(ShieldImpact);

void ShieldImpact::Start() {
    GetOwner().GetComponent<ComponentParticleSystem>()->PlayChildParticles();
}

void ShieldImpact::Update() {
    if (lifeTime > 0) {
        lifeTime -= Time::GetDeltaTime();
    }
    else {
        GameplaySystems::DestroyGameObject(&GetOwner());
    }
}