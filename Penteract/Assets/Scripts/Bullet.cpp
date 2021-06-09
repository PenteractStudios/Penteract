#include "Bullet.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(Bullet) {
};

GENERATE_BODY_IMPL(Bullet);

void Bullet::Start() {

}

void Bullet::Update() {

}

void Bullet::OnCollision(GameObject& collidedWith) {
    if (collidedWith.name == "Fang" || collidedWith.name == "Onimaru") return;
    GameplaySystems::DestroyGameObject(&GetOwner());
}