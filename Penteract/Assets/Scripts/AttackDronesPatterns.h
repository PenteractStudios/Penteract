#pragma once

#include "Scripting/Script.h"

class AttackDronesPatterns : public Script
{
	GENERATE_BODY(AttackDronesPatterns);

public:

	void Start() override;
	void Update() override;

};

