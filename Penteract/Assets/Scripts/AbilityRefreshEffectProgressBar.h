#pragma once

#include "Scripting/Script.h"

class ComponentProgressBar;

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
	bool debugPlay = false;
	UID progressBarUID = 0;

private:
	GameObject* progressBarGO;
	ComponentProgressBar* progressBar;
	float currentTime = 0.0f;
	bool animationFinished = true;
	//float3 effectScaleVector = float3(0, 0, 0);
	//float3 originalEffectScaleVector = float3(0, 0, 0);
	//float3 originalScaleVector = float3(0, 0, 0);
	//float effectTimer = 0;

	//float originalAlpha = 0;
	//float3 originalColor = float3(0, 0, 0);

	//bool isPlaying = false;

	//ComponentImage* effectMember1 = nullptr;
	//ComponentTransform2D* skillObjTransform2D = nullptr;
	//ComponentTransform2D* effectTransform2D = nullptr;
};

