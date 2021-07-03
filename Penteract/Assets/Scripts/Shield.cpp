#include "Shield.h"

#include "GameObject.h"
#include "GameplaySystems.h"

#include "Onimaru.h"

EXPOSE_MEMBERS(Shield) {
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruUID),
	MEMBER(MemberType::INT, max_charges),
	MEMBER(MemberType::INT, cooldownCharge)
};

GENERATE_BODY_IMPL(Shield);

void Shield::Start() {
	//onimaruGO = GameplaySystems::GetGameObject(onimaruUID);
	//if (onimaruGO) onimaru = GET_SCRIPT(onimaruGO, Onimaru);
}

void Shield::Update() {}

void Shield::InitShield() {
	num_charges = 0;
	isActive = true;
}

void Shield::FadeShield() {
	isActive = false;
	totalCooldown = cooldownCharge * num_charges;
}

void Shield::OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance) {

	if ((collidedWith.name == "RangerProjectile" || collidedWith.name == "MeleePunch") && isActive) {
		GameplaySystems::DestroyGameObject(&collidedWith);
		num_charges++;
	}
}
