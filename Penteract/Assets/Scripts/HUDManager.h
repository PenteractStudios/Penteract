#pragma once

#include "Scripting/Script.h"

class HUDManager : public Script
{
	GENERATE_BODY(HUDManager);

public:

	void Start() override;
	void Update() override;

};

