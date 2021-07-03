#pragma once

#include "Scripting/Script.h"
#include <vector>

class ResourcePrefab;

class UltimateFang : public Script
{
	GENERATE_BODY(UltimateFang);

public:

	void Start() override;
	void Update() override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;
	void StartUltiamte();
	void EndUltiamte();

private:
	Quat DirectionToQuat(float3 dir);

public:
	float radius = 5.0f;
	UID fangBulletUID = 0;
	UID fangTrailUID = 0;

private:
	ResourcePrefab* trail = nullptr;
	ResourcePrefab* bullet = nullptr;

	bool tickOn = false;
	float tickCurrent = 0.2f;
	float tickDuration = 0.2f;
	std::vector<GameObject> collisionedGameObject;

	bool active = false;
};
