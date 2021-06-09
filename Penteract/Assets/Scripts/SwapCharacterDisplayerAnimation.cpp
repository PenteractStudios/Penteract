#include "SwapCharacterDisplayerAnimation.h"

#include "GameplaySystems.h"

EXPOSE_MEMBERS(SwapCharacterDisplayerAnimation) {
	MEMBER(MemberType::BOOL, debugPlay),
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

	MEMBER(MemberType::GAME_OBJECT_UID, fangMainDisplayerUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fangPrimaryUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fangAlternativeUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruMainDisplayerUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruPrimaryUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruAlternativeUID)
};

GENERATE_BODY_IMPL(SwapCharacterDisplayerAnimation);

void SwapCharacterDisplayerAnimation::Start() {
	firstBarEffect = GameplaySystems::GetGameObject(firstBarEffectUID);
	secondBarEffect = GameplaySystems::GetGameObject(firstBarEffectUID);
	thirdBarEffect = GameplaySystems::GetGameObject(firstBarEffectUID);
}

void SwapCharacterDisplayerAnimation::Update() {

	if (!firstBarEffect || !secondBarEffect || !thirdBarEffect) {
		return;
	}
	/*Debug::Log(std::to_string(currentTime).c_str());*/

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

	// Swap panels
	if (!animationSwapFinished) {
		
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

void SwapCharacterDisplayerAnimation::Play() {
	currentTime = 0;
	animationPopUpFinished = false;
	animationSwapFinished = false;
	firstBarEffect->Disable();
	secondBarEffect->Disable();
	thirdBarEffect->Disable();
}