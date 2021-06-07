#include "AbilityRefreshEffectProgressBar.h"

#include "GameplaySystems.h"
#include "Components/UI/ComponentProgressBar.h"

EXPOSE_MEMBERS(AbilityRefreshEffectProgressBar) {
	MEMBER(MemberType::FLOAT, duration),
	MEMBER(MemberType::BOOL, debugPlay),
	MEMBER(MemberType::GAME_OBJECT_UID, progressBarUID)
};

GENERATE_BODY_IMPL(AbilityRefreshEffectProgressBar);

void AbilityRefreshEffectProgressBar::Start() {
	progressBarGO = GameplaySystems::GetGameObject(progressBarUID);
	if (progressBarGO != nullptr) {
		progressBar = progressBarGO->GetComponent<ComponentProgressBar>();
		progressBar->SetValue(1);
	}
}

void AbilityRefreshEffectProgressBar::Update() {
	if (!progressBar) {
		return;
	}

	if (debugPlay) {
		debugPlay = false;
		Play();
	}

	if (!animationFinished) {
		if (currentTime <= duration) {
			progressBar->SetValue(currentTime / duration);
		}
		else{
			animationFinished = true;
			progressBar->SetValue(1);
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
	animationFinished = false;
}

void AbilityRefreshEffectProgressBar::ResetBar()
{
	if (!progressBar) {
		return;
	}
	progressBar->SetValue(0);
}
