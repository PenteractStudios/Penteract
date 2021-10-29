#pragma once

#include "Scripting/Script.h"

class DukeDoor : public Script
{
	GENERATE_BODY(DukeDoor);

public:

	void Start() override;
	void Update() override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;

public:
	UID dukeUID = 0;
	UID doorObstacleUID = 0;
	UID canvasHUDUID = 0;
};

