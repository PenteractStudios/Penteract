#pragma once

#include "Scripting/Script.h"

class DukeDeathTrigger : public Script
{
	GENERATE_BODY(DukeDeathTrigger);

public:

	void Start() override;
	void Update() override;

};

