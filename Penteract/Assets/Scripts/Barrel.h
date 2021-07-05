#pragma once

#include "Scripting/Script.h"

class ComponentSphereCollider;
class CameraController;

class Barrel : public Script
{
	GENERATE_BODY(Barrel);

public:

	void Start() override;
	void Update() override;

	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle) override;

public:
	UID barrelUID = 0;
	UID cameraUID = 0;
	ComponentSphereCollider* barrelCollider = nullptr;
	CameraController* cameraController = nullptr;
};
