#pragma once

#include "Scripting/Script.h"

class GameObject;

class SpawnPointController : public Script {
	GENERATE_BODY(SpawnPointController);

public:
	void Start() override;
	void Update() override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance) override;

private:
	GameObject* gameObject = nullptr;
};
