#pragma once

#include "Scripting/Script.h"

class ComponentImage;
class GameObject;

class LowHPWarning : public Script
{
	GENERATE_BODY(LowHPWarning);

public:

	UID effectUID = 0;
	float effectTime = 1.0f; // In seconds

public:

	void Start() override;
	void Update() override;
	void Play();
	void Stop();

private:
	bool playing = false;
	float fadeOutTime = 0.0f;
	float fadeOutCurrentTime = 0.0f;
	float effectCurrentTime = 0.0f;
	GameObject* effect = nullptr;
	ComponentImage* vignette = nullptr;
};

