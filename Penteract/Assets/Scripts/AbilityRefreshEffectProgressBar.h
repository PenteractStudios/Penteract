#pragma once

#include "Scripting/Script.h"

class ComponentProgressBar;
class ComponentImage;

class AbilityRefreshEffectProgressBar : public Script
{
	GENERATE_BODY(AbilityRefreshEffectProgressBar);

public:

	void Start() override;
	void Update() override;
	void Play();
	void ResetBar();

public:
	float duration = .2f;
	float breakTime = .2f;					// Time between the progress bar loading up and the "pop" effect. This is the time that needs to pass AFTER the progress bar loads up
	float durationFirstEmission = .1f;
	float breakTimeBetweenFirstAndSecond = .1f;
	float durationSecondEmission = .1f;
	float breakTimeBetweenSecondAndThird = .1f;
	float durationThirdEmission = .1f;
	float3 minScale = float3(1, 0, 1);		// Scales for the pop up effect
	float3 maxScale = float3(1, 1, 1);		// Scales for the pop up effect
	float minTransparency = 0;
	float maxTransparency = 1;
	bool debugPlay = false;

	UID progressBarUID = 0;
	UID firstBarEffectUID;
	UID secondBarEffectUID;
	UID thirdBarEffectUID;


private:
	void IncreaseOverTime(GameObject* image, float currentTime, float maxTime);

private:
	GameObject* progressBarGO;
	GameObject* firstBarEffect;
	GameObject* secondBarEffect;
	GameObject* thirdBarEffect;
	ComponentProgressBar* progressBar;

	float currentTime = 0.0f;
	bool animationLoadUpFinished = true;
	bool animationPopUpFinished = true;
};

