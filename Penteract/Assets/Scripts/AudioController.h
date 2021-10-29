#pragma once

#include "Scripting/Script.h"

class AudioController : public Script
{
	GENERATE_BODY(AudioController);

public:

	void Start() override;
	void Update() override;


	// Start Volume for Start Scene
	float mainGain = 1.0f;
	float musicGain = 1.0f;
	float sfxGain = 1.0f;

};

