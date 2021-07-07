#pragma once

#include "Scripting/Script.h"

class LaserTurret : public Script
{
	GENERATE_BODY(LaserTurret);

public:

	void Start() override;
	void Update() override;

};

