#include "SwapCharacterDisplayerAnimation.h"

#include "GameplaySystems.h"

EXPOSE_MEMBERS(SwapCharacterDisplayerAnimation) {
	MEMBER(MemberType::BOOL, debugPlay),
	MEMBER(MemberType::BOOL, isFang),
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
	MEMBER(MemberType::FLOAT, maxTransparency),
	MEMBER(MemberType::FLOAT, breakTimeForSwap),
	MEMBER(MemberType::FLOAT, swappingDuration),

	MEMBER(MemberType::GAME_OBJECT_UID, fangMainDisplayerUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fangPrimaryUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fangAlternativeUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruMainDisplayerUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruPrimaryUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruAlternativeUID),
	MEMBER(MemberType::FLOAT, positionToLerpSwapMain),
	MEMBER(MemberType::FLOAT, positionToLerpSwapAlternative)
};

GENERATE_BODY_IMPL(SwapCharacterDisplayerAnimation);

void SwapCharacterDisplayerAnimation::Start() {
	firstBarEffect = GameplaySystems::GetGameObject(firstBarEffectUID);
	secondBarEffect = GameplaySystems::GetGameObject(secondBarEffectUID);
	thirdBarEffect = GameplaySystems::GetGameObject(thirdBarEffectUID);

	fangMainDisplayer = GameplaySystems::GetGameObject(fangMainDisplayerUID);
	fangPrimary = GameplaySystems::GetGameObject(fangPrimaryUID);
	fangAlternative = GameplaySystems::GetGameObject(fangAlternativeUID);
	onimaruMainDisplayer = GameplaySystems::GetGameObject(onimaruMainDisplayerUID);
	onimaruPrimary = GameplaySystems::GetGameObject(onimaruPrimaryUID);
	onimaruAlternative = GameplaySystems::GetGameObject(onimaruAlternativeUID);

	if (fangMainDisplayer != nullptr) {
		ComponentTransform2D* transform = fangPrimary->GetComponent<ComponentTransform2D>();
		if (transform != nullptr) {
			initialPositionMainCanvas = transform->GetPosition();
		}

		transform = fangAlternative->GetComponent<ComponentTransform2D>();
		if (transform != nullptr) {
			initialPositionAlternativeCanvas = transform->GetPosition();
		}
	}
}

void SwapCharacterDisplayerAnimation::Update() {

	if (!firstBarEffect || !secondBarEffect || !thirdBarEffect) {
		return;
	}

	if (debugPlay) {
		debugPlay = false;
		Play();
	}


	// Popup flash
	if (!animationPopUpFinished) {
		//float durationAndBreakTimes = durationLoadUp + breakTimeForPopups;
		float durationAndBreakTimes = 0;
		if (currentTime > durationAndBreakTimes && currentTime <= (durationAndBreakTimes + durationFirstEmission)) {		// First popup
			firstBarEffect->Enable();
			IncreaseOverTime(firstBarEffect, currentTime, durationFirstEmission);
		}
		else if (currentTime > (durationAndBreakTimes + durationFirstEmission)) {
			firstBarEffect->Disable();
		}

		durationAndBreakTimes += breakTimeBetweenFirstAndSecond + durationFirstEmission;
		if (currentTime > durationAndBreakTimes && currentTime <= (durationAndBreakTimes + durationSecondEmission)) {		// Second popup
			secondBarEffect->Enable();
			IncreaseOverTime(secondBarEffect, currentTime, durationAndBreakTimes + durationSecondEmission);
		}
		else if (currentTime > (durationAndBreakTimes + durationSecondEmission)) {
			secondBarEffect->Disable();
		}

		durationAndBreakTimes += breakTimeBetweenSecondAndThird;
		if (currentTime > durationAndBreakTimes && currentTime <= (durationAndBreakTimes + durationThirdEmission)) {		// Third popup
			thirdBarEffect->Enable();
			IncreaseOverTime(thirdBarEffect, currentTime, durationAndBreakTimes + durationThirdEmission);
		}
		else if (currentTime > (durationAndBreakTimes + durationThirdEmission)) {
			animationPopUpFinished = true;
			thirdBarEffect->Disable();
		}
	}

	Debug::Log(std::to_string(currentTime).c_str());

	// Swap panels
	if (!animationSwapFinished) {
		// Swaps first character
		float durationAndBreakTimes = breakTimeForSwap + swappingDuration;
		if (currentTime > breakTimeForSwap && currentTime <= durationAndBreakTimes) {
			GameObject* target = isFang ? fangPrimary : onimaruPrimary;
			SwapAnimation(target, true, false, currentTime, durationAndBreakTimes);
			SwapAnimation(target == fangPrimary ? onimaruAlternative : fangAlternative, false, false, currentTime, durationAndBreakTimes);
		}
		// Swaps second character
		else if(currentTime > durationAndBreakTimes && currentTime <= (durationAndBreakTimes + swappingDuration)) {
			GameObject* target = isFang ? fangPrimary : onimaruPrimary;
			SwapAnimation(target, true, true, currentTime, durationAndBreakTimes + swappingDuration);
			SwapAnimation(target == fangPrimary ? onimaruAlternative : fangAlternative, false, true, currentTime, durationAndBreakTimes + swappingDuration);
		}
		else if(currentTime > (durationAndBreakTimes + swappingDuration)) {
			animationSwapFinished = true;
		}
	}

	currentTime += Time::GetDeltaTime();
}

void SwapCharacterDisplayerAnimation::IncreaseOverTime(GameObject* go, float currentTime, float maxTime)
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

void SwapCharacterDisplayerAnimation::SwapAnimation(GameObject* targetCanvas, bool isMain, bool playReverse, float currentTime, float maxTime)
{
	ComponentTransform2D* transform = targetCanvas->GetComponent<ComponentTransform2D>();

	if (playReverse) {
		transform->SetScale(float3::Lerp(float3(initialScaleCanvas.x, 0, initialScaleCanvas.z), initialScaleCanvas,  currentTime / maxTime));
		transform->SetPosition(float3::Lerp(float3(transform->GetPosition().x, isMain ? positionToLerpSwapMain : positionToLerpSwapAlternative, transform->GetPosition().z), isMain ? initialPositionMainCanvas : initialPositionAlternativeCanvas, currentTime / maxTime));
	}
	else {
		transform->SetScale(float3::Lerp(initialScaleCanvas, float3(initialScaleCanvas.x, 0, initialScaleCanvas.z), currentTime / maxTime));
		transform->SetPosition(float3::Lerp(transform->GetPosition(), float3(transform->GetPosition().x, isMain ? positionToLerpSwapMain : positionToLerpSwapAlternative, transform->GetPosition().z), currentTime / maxTime));
	}
}

void SwapCharacterDisplayerAnimation::Play() {
	currentTime = 0;
	animationPopUpFinished = false;
	animationSwapFinished = false;
	firstBarEffect->Disable();
	secondBarEffect->Disable();
	thirdBarEffect->Disable();
	isFang = !isFang;
}