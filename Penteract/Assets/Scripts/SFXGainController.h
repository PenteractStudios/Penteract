#pragma once

#include "Scripting/Script.h"

class ComponentSlider;

class SFXGainController : public Script
{
	GENERATE_BODY(SFXGainController);

public:

	void Start() override;
	void Update() override;
	void OnValueChanged() override;

private:
	ComponentSlider* slider = nullptr;
	bool isUpdated = false;
};

