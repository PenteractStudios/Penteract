#include "AttackDroneProjectile.h"

#include "Components/ComponentTransform.h"

#include "GameplaySystems.h"

EXPOSE_MEMBERS(AttackDroneProjectile) {
    MEMBER(MemberType::FLOAT, speed)
};

GENERATE_BODY_IMPL(AttackDroneProjectile);

void AttackDroneProjectile::Start() {
	transform = GetOwner().GetComponent<ComponentTransform>();
}

void AttackDroneProjectile::Update() {
	if (transform) transform->SetGlobalPosition(transform->GetGlobalPosition() + transform->GetFront() * speed * Time::GetDeltaTime());

	if (currentTime >= destroyTime) {
		GameplaySystems::DestroyGameObject(&GetOwner());
	}
	else {
		currentTime += Time::GetDeltaTime();
	}
}

void AttackDroneProjectile::SetDestroyTime(float newDestroyTime) {
    destroyTime = newDestroyTime;
}

void AttackDroneProjectile::SetSpeed(float newSpeed) {
	speed = newSpeed;
}
