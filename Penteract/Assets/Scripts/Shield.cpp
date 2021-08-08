#include "Shield.h"

#include "GameObject.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(Shield) {
	MEMBER(MemberType::INT, maxCharges),
	MEMBER(MemberType::FLOAT, chargeCooldown),
	MEMBER_SEPARATOR("Debug only"),
	MEMBER(MemberType::INT, currentAvailableCharges)
};

GENERATE_BODY_IMPL(Shield);

void Shield::Start() {
	currentAvailableCharges = maxCharges;
}

void Shield::Update() {}

void Shield::InitShield() {
	isActive = true;
}

void Shield::FadeShield() {
	isActive = false;
}

void Shield::OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle) {
	if ((collidedWith.name == "RangerProjectile" || collidedWith.name == "MeleePunch") && isActive) {
		GameplaySystems::DestroyGameObject(&collidedWith);
		currentAvailableCharges--;
	}
}