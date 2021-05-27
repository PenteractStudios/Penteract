#pragma once

#include "Scripting/Script.h"

class ComponentImage;
class ComponentTransform2D;

class HealthLostFeedback : public Script {
	GENERATE_BODY(HealthLostFeedback);

public:

	void Start() override;
	void Update() override;
	void Play();
public:
	float growthDuration = 0.25f;
	float fadeOutDuration = 1.5f;
	float growthMaxScale = 0.3f;
	UID imageUID = 0;
	ComponentImage* image = nullptr;
	ComponentTransform2D* transform = nullptr;
	float2 originalSize = { 0,0 };
private:
	bool playing;

	float growthTimer = 0;
	float fadeOutTimer = 0;

};

