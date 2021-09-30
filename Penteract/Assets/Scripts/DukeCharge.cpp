#include "DukeCharge.h"

#include "GameObject.h"
#include "AIDuke.h"

EXPOSE_MEMBERS(DukeCharge) {
    // Add members here to expose them to the engine. Example:
    // MEMBER(MemberType::BOOL, exampleMember1),
    // MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
    MEMBER(MemberType::GAME_OBJECT_UID, dukeUID)
};

GENERATE_BODY_IMPL(DukeCharge);

void DukeCharge::Start() {
    GameObject* duke = GameplaySystems::GetGameObject(dukeUID);
    if (duke) aiDuke = GET_SCRIPT(duke, AIDuke);
}

void DukeCharge::Update() {

}

void DukeCharge::OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle) {
    if (collidedWith.name == "Fang" || collidedWith.name == "Onimaru") {
        aiDuke->dukeCharacter.UpdateCharge(true);
    }
}