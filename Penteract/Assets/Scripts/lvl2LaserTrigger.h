#pragma once

#include "Scripting/Script.h"

class lvl2LaserTrigger : public Script
{
	GENERATE_BODY(lvl2LaserTrigger);

public:

	void Start() override;
	void Update() override;

};

