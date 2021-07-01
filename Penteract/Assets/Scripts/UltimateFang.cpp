#include "UltimateFang.h"

#include "GameObject.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(UltimateFang) {
	MEMBER(MemberType::FLOAT, radius),
	MEMBER(MemberType::FLOAT, duration),
	MEMBER(MemberType::PREFAB_RESOURCE_UID, fangBulletUID),
	MEMBER(MemberType::PREFAB_RESOURCE_UID, fangTrailUID)
};

GENERATE_BODY_IMPL(UltimateFang);

void UltimateFang::Start() {
	ComponentSphereCollider* sphereCollider = GetOwner().GetComponent<ComponentSphereCollider>();
	sphereCollider->radius = radius;

	trail = GameplaySystems::GetResource<ResourcePrefab>(fangTrailUID);
	bullet = GameplaySystems::GetResource<ResourcePrefab>(fangBulletUID);

	active = false;
}

void UltimateFang::Update() {
	if (!active) return;
	if (currentDuration >= 0) {
		currentDuration -= Time::GetDeltaTime();

		tickCurrent -= Time::GetDeltaTime();
		if (tickCurrent <= 0) {
			tickOn = true;
			tickCurrent = tickDuration;


			/*float3 aux = onimaruDirection * float3(0.0f, 0.0f, 1.0f);
			aux *= speed * Time::GetDeltaTime();
			float3 newPosition = transform->GetGlobalPosition();
			newPosition += aux;
			transform->SetGlobalPosition(newPosition);*/

			if (collisionedGameObject.size() > 0) {
				ComponentTransform* transformOwner = GetOwner().GetComponent<ComponentTransform>();
				ComponentTransform* transformTarget = collisionedGameObject.front().GetComponent<ComponentTransform>();
				GameplaySystems::Instantiate(bullet, transformOwner->GetGlobalPosition(), transformTarget->GetGlobalRotation().Inverted());
				GameplaySystems::Instantiate(trail, transformOwner->GetGlobalPosition(), transformTarget->GetGlobalRotation().Inverted());
			}
		}
		collisionedGameObject.clear();
		//Debug::Log("Update Clear");
	}
	else {
		//GameplaySystems::DestroyGameObject(&GetOwner());
		active = false;
		Debug::Log("Finish ultimate");
	}
}

void UltimateFang::OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance) {
	if (!active) return;
	//if (tickOn) {
		tickOn = false;
		if (collidedWith.name == "MeleeGrunt" || collidedWith.name == "RangeGrunt") {
			collisionedGameObject.push_back(collidedWith);
			Debug::Log(collidedWith.name.c_str());
		}
	//}
}

void UltimateFang::StartUltiamte()
{
	active = true;
	currentDuration = duration;
	tickCurrent = tickDuration;
}
