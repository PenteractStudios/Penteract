#include "RangerProjectileScript.h"

#include "GameplaySystems.h"
#include "PlayerController.h"
#include "Components/ComponentTransform.h"

EXPOSE_MEMBERS(RangerProjectileScript) {
	MEMBER(MemberType::INT, speed),
		MEMBER(MemberType::FLOAT, life)
};

GENERATE_BODY_IMPL(RangerProjectileScript);

void RangerProjectileScript::Start() {

}

void RangerProjectileScript::Update() {
	if (life >= 0) {
		life -= Time::GetDeltaTime();
		ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
		float3 aux = transform->GetGlobalRotation() * float3(0.0f, 0.0f, 1.0f);

		if (itsVertical) {
			if (!firstTime) {
				transform->SetGlobalRotation(Quat::RotateAxisAngle(aux, (pi / 2)).Mul(transform->GetGlobalRotation()));
				firstTime = true;
			}
		}

		aux *= speed * Time::GetDeltaTime();
		float3 newPosition = transform->GetGlobalPosition();
		newPosition += aux;
		transform->SetGlobalPosition(newPosition);
	} else {
		GameplaySystems::DestroyGameObject(&GetOwner());
	}
}

void RangerProjectileScript::OnCollision(const GameObject& collidedWith) {
	//There appears to be an issue, projectiles colliding with each other, for now it should only collide with player (walls later)
	if (collidedWith.name == "Onimaru" || collidedWith.name == "Fang") {
			GameplaySystems::DestroyGameObject(&GetOwner());
	}
}
