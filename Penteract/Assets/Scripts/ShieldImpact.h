#pragma once

#include "Scripting/Script.h"

class ShieldImpact : public Script
{
	GENERATE_BODY(ShieldImpact);

public:

	void Start() override;
	void Update() override;

public:
	float lifeTime = 1.0f;
};

