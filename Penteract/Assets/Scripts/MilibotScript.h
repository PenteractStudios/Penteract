#pragma once

#include "Scripting/Script.h"

class ComponentTransform;
class GameObject; 

class MilibotScript : public Script
{
	GENERATE_BODY(MilibotScript);

public:

	void Start() override;
	void Update() override;

public:
	float angle = 0.0f;
	float rotationSpeed = 0.0f;
	float positionSpeed = 0.0f;
	float motionTime = 0.0f;

private:
	float3 newPosition = float3(0.0f, 0.0f, 0.0f);
	ComponentTransform* transform = nullptr;
	float restMotionTime = 0.0;
	bool isUp = false;
};

