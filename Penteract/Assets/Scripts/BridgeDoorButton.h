#pragma once

#include "Scripting/Script.h"

class GameObject;

class BridgeDoorButton : public Script
{
	GENERATE_BODY(BridgeDoorButton);

public:
	UID bridgeDoorUID = 0;

public:

	void Start() override;
	void Update() override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;

private:
	GameObject* gameObject = nullptr;
	Script* script = nullptr;
};

