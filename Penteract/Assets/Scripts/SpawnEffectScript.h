#pragma once

#include "Scripting/Script.h"
class ComponentTransform;

class SpawnEffectScript : public Script
{
	GENERATE_BODY(SpawnEffectScript);

public:

	void Start() override;
	void Update() override;

public:
	float ground = 3.0f;
	ComponentTransform* mainTransform = nullptr;
};

