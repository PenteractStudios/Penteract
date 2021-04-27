#pragma once

#include "Script.h"

#include "Math/float3.h"

class GameObject;
class ComponentTransform;
class TreeFloating : public Script
{
	GENERATE_BODY(TreeFloating);

public:

	void Start() override;
	void Update() override;

public:
	GameObject* gameObject = nullptr;

private:
	float offset = 0.f;
	float3 initialPosition = float3(0,0,0);
	ComponentTransform* transform = nullptr;
};

