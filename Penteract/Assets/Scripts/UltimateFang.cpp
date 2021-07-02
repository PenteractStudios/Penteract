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
		}
		else {
			tickOn = false;
			if (collisionedGameObject.size() > 0) {
				int i = rand() % collisionedGameObject.size(); // select random enemi from the list
				ComponentTransform* transformOwner = GetOwner().GetComponent<ComponentTransform>();
				ComponentTransform* transformTarget = collisionedGameObject[i].GetComponent<ComponentTransform>();

				float3 posFang = transformOwner->GetGlobalPosition();
				float3 posTarget = transformTarget->GetGlobalPosition();
				float3 dir = float3(posTarget.x - posFang.x, posTarget.y - posFang.y, posTarget.z - posFang.z).Normalized();
				GameplaySystems::Instantiate(bullet, transformOwner->GetGlobalPosition(), DirectionToQuat(dir));
				GameplaySystems::Instantiate(trail, transformOwner->GetGlobalPosition(), DirectionToQuat(dir));
			}
		}
		collisionedGameObject.clear();
	}
	else {
		active = false;
		Debug::Log("Finish ultimate");
	}
}

Quat UltimateFang::DirectionToQuat(float3 dir) {
	float angle = atan2(dir.x, dir.z);
	float qx = 0;
	float qy = 1 * sin(angle / 2);
	float qz = 0;
	float qw = cos(angle / 2);
	return Quat(qx, qy, qz, qw);
}

void UltimateFang::OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance) {
	if (!active) return;
	if (tickOn) {
		//tickOn = false;
		if (collidedWith.name == "MeleeGrunt" || collidedWith.name == "RangeGrunt") {
			collisionedGameObject.push_back(collidedWith);
			Debug::Log(collidedWith.name.c_str());
		}
	}
}

void UltimateFang::StartUltiamte()
{
	active = true;
	currentDuration = duration;
	tickCurrent = tickDuration;
}
