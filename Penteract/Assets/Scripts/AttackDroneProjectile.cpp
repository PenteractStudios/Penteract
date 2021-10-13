#include "AttackDroneProjectile.h"

#include "Components/ComponentTransform.h"

#include "GameplaySystems.h"

EXPOSE_MEMBERS(AttackDroneProjectile) {
	MEMBER(MemberType::FLOAT, speed),
	MEMBER(MemberType::FLOAT, destroyTime),
	MEMBER(MemberType::FLOAT, destroyTimeOnCollision)
};

GENERATE_BODY_IMPL(AttackDroneProjectile);

void AttackDroneProjectile::Start() {
	transform = GetOwner().GetComponent<ComponentTransform>();
}

void AttackDroneProjectile::Update() {
	if (transform && !mustStopMovement) transform->SetGlobalPosition(transform->GetGlobalPosition() + transform->GetFront() * speed * Time::GetDeltaTime());

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

void AttackDroneProjectile::Collide() {
	// In case it has it
	ComponentSphereCollider* collider = GetOwner().GetComponent<ComponentSphereCollider>();
	if (collider) {
		collider->Disable();
	}

	ComponentMeshRenderer* mesh = GetOwner().GetComponent<ComponentMeshRenderer>();
	if (mesh) {
		mesh->Disable();
	}

	mustStopMovement = true;
	currentTime = 0.0f;
	destroyTime = destroyTimeOnCollision;
}
