#pragma once

#include "Scripting/Script.h"

class VehicleLine : public Script
{
	GENERATE_BODY(VehicleLine);

public:

	void Start() override;
	void Update() override;

};

