#pragma once

#include "Scripting/Script.h"

class GameObject;

class UpdateCheckpointVariable : public Script
{
	GENERATE_BODY(UpdateCheckpointVariable);

public:

	void Start() override;
	void Update() override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;

public:
	int actualLevel = -1;
	int newCheckpoint = -1;
};

