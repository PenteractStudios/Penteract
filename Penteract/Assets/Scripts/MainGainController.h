#pragma once

#include "Scripting/Script.h"

class ComponentSlider;

class MainGainController : public Script
{
	GENERATE_BODY(MainGainController);

public:

	void Start() override;
	void Update() override;
	void OnValueChanged() override;

private:
	ComponentSlider* slider = nullptr;
	bool isUpdated = false;
};

