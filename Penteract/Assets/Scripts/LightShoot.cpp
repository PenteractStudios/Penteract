#include "LightShoot.h"
#include "GameObject.h"
#include "GameplaySystems.h"
EXPOSE_MEMBERS(LightShoot) {
	MEMBER(MemberType::INT, speed),
		MEMBER(MemberType::FLOAT, life)
};

GENERATE_BODY_IMPL(LightShoot);

void LightShoot::Start() {
}

void LightShoot::Update() {
	if (life >= 0) {
		life -= Time::GetDeltaTime();
		ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
		float3 aux = onimaruDirection * float3(0.0f, 0.0f, 1.0f);
		aux *= speed * Time::GetDeltaTime();
		float3 newPosition = transform->GetGlobalPosition();
		newPosition += aux;
		transform->SetGlobalPosition(newPosition);
	}
	else {
		GameplaySystems::DestroyGameObject(&GetOwner());
	}
}

void LightShoot::SetOnimaruDirection(Quat direction) {
	onimaruDirection = direction;
}