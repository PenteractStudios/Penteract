#pragma once

#include "Scripting/Script.h"

class GameObject;

class WinCondition : public Script {
	GENERATE_BODY(WinCondition);

public:
	void Start() override;
	void Update() override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;

public:
	UID sceneUID = 0;

private:
	GameObject* gameObject = nullptr;
};
