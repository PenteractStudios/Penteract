#pragma once

#include "Scripting/Script.h"

class GameObject;

class TriggerEnable : public Script
{
	GENERATE_BODY(TriggerEnable);

public:

	void Start() override;
	void Update() override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;

public:
	UID objectToEnableUID = 0;
	UID objectToDisableUID = 0;

private:
	GameObject* objectToEnable = nullptr;
	GameObject* objectToDisable = nullptr;
};

