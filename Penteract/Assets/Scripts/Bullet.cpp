#include "Bullet.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(Bullet) {
    // Add members here to expose them to the engine. Example:
    // MEMBER(MemberType::BOOL, exampleMember1),
    // MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
    // MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
};

GENERATE_BODY_IMPL(Bullet);

void Bullet::Start() {

}

void Bullet::Update() {

}

void Bullet::OnCollision(const GameObject& collidedWith) {
    if (collidedWith.name == "Fang" || collidedWith.name == "Onimaru") return;
    GameplaySystems::DestroyGameObject(&GetOwner());
}