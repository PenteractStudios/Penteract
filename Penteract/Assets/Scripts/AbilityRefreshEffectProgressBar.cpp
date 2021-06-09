#include "AbilityRefreshEffectProgressBar.h"

#include "GameplaySystems.h"
#include "Components/UI/ComponentProgressBar.h"
#include "Components/UI/ComponentImage.h"

#include <math.h> 

EXPOSE_MEMBERS(AbilityRefreshEffectProgressBar) {
	MEMBER(MemberType::FLOAT, duration),
	MEMBER(MemberType::BOOL, debugPlay),
	MEMBER(MemberType::GAME_OBJECT_UID, progressBarUID),
	MEMBER(MemberType::GAME_OBJECT_UID, firstBarEffectUID),
	MEMBER(MemberType::GAME_OBJECT_UID, secondBarEffectUID),
	MEMBER(MemberType::GAME_OBJECT_UID, thirdBarEffectUID),
	MEMBER(MemberType::FLOAT, durationFirstEmission),
	MEMBER(MemberType::FLOAT, breakTimeBetweenFirstAndSecond),
	MEMBER(MemberType::FLOAT, durationSecondEmission),
	MEMBER(MemberType::FLOAT, breakTimeBetweenSecondAndThird),
	MEMBER(MemberType::FLOAT, durationThirdEmission),
	MEMBER(MemberType::FLOAT3, minScale),
	MEMBER(MemberType::FLOAT3, maxScale),
	MEMBER(MemberType::FLOAT, minTransparency),
	MEMBER(MemberType::FLOAT, maxTransparency)
};

GENERATE_BODY_IMPL(AbilityRefreshEffectProgressBar);

void AbilityRefreshEffectProgressBar::Start() {
	progressBarGO = GameplaySystems::GetGameObject(progressBarUID);
	if (progressBarGO != nullptr) {
		progressBar = progressBarGO->GetComponent<ComponentProgressBar>();
		progressBar->SetValue(1);
	}

	firstBarEffect = GameplaySystems::GetGameObject(firstBarEffectUID);
	secondBarEffect = GameplaySystems::GetGameObject(firstBarEffectUID);
	thirdBarEffect = GameplaySystems::GetGameObject(firstBarEffectUID);
}

void AbilityRefreshEffectProgressBar::Update() {
	if (!progressBar) {
		return;
	}

	if (debugPlay) {
		debugPlay = false;
		Play();
	}

	if (!animationLoadUpFinished) {
		if (currentTime <= duration) {
			progressBar->SetValue(currentTime / duration);
		}
		else{
			animationLoadUpFinished = true;
			progressBar->SetValue(1);
		}
	}
	Debug::Log(std::to_string(currentTime).c_str());
	if (!animationPopUpFinished) {
		float durationAndBreakTimes = duration + breakTime;
		if (currentTime > durationAndBreakTimes && currentTime <= (durationAndBreakTimes + durationFirstEmission)) {		// First popup
			firstBarEffect->Enable();
			IncreaseOverTime(firstBarEffect, currentTime, duration + breakTime + durationFirstEmission);
		}

		durationAndBreakTimes += breakTimeBetweenFirstAndSecond + durationFirstEmission;
		if (currentTime > durationAndBreakTimes && currentTime <= (durationAndBreakTimes + durationSecondEmission)) {		// Second popup
			secondBarEffect->Enable();
			IncreaseOverTime(secondBarEffect, currentTime, durationAndBreakTimes + durationSecondEmission);
		}
	}
	
	currentTime += Time::GetDeltaTime();
}

void AbilityRefreshEffectProgressBar::Play() {
	if (!progressBar) {
		return;
	}

	currentTime = 0;
	progressBar->SetValue(0);
	debugPlay = false;
	animationLoadUpFinished = false;
	animationPopUpFinished = false;
}

void AbilityRefreshEffectProgressBar::ResetBar()
{
	if (!progressBar) {
		return;
	}
	progressBar->SetValue(0);
}

void AbilityRefreshEffectProgressBar::IncreaseOverTime(GameObject* go, float currentTime, float maxTime)
{
	if (go == nullptr) {
		return;
	}
	float factor = currentTime / maxTime;

	ComponentImage* image = go->GetComponent<ComponentImage>();
	if (image) {
		image->SetColor(float4::Lerp(float4(image->GetColor().xyz(), maxTransparency), float4(image->GetColor().xyz(), minTransparency), factor));
	}

	ComponentTransform2D* transform = go->GetComponent<ComponentTransform2D>();
	if (transform) {
		transform->SetScale(float3::Lerp(minScale, maxScale, factor));
	}
}
