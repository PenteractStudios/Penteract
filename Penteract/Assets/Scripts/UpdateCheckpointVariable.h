#pragma once

#include "Scripting/Script.h"

class GameObject;

class UpdateCheckpointVariable : public Script
{
	GENERATE_BODY(UpdateCheckpointVariable);

public:

	void Start() override;
	void Update() override;
	void OnCollision(GameObject& /*collidedWith*/, float3 /*collisionNormal*/, float3 /*penetrationDistance*/, void* /*particle*/) override;

public:
	int newLevel = 1;
	int newCheckpoint = 0;

private:
	GameObject* gameObjectTrigger = nullptr;
};

