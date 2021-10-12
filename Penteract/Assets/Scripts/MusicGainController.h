#pragma once

#include "Scripting/Script.h"

class ComponentSlider;

class MusicGainController : public Script
{
	GENERATE_BODY(MusicGainController);

public:

	void Start() override;
	void Update() override;
	void OnValueChanged() override;

private:
	ComponentSlider* slider = nullptr;
	bool isUpdated = false;
};


