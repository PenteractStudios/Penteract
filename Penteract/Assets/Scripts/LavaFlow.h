#pragma once

#include "Scripting/Script.h"

class ComponentMeshRenderer;

class LavaFlow : public Script
{
	GENERATE_BODY(LavaFlow);

public:

	void Start() override;
	void Update() override;

public:
	float speed = 0.2f;
	bool randomStart = false;

private:
	ComponentMeshRenderer* mesh;
};

