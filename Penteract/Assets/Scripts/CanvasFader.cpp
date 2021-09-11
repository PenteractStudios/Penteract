#include "CanvasFader.h"

#include "GameplaySystems.h"
#include "GameObject.h"
#include "Components/UI/ComponentImage.h"

EXPOSE_MEMBERS(CanvasFader) {
	// Add members here to expose them to the engine. Example:
	// MEMBER(MemberType::BOOL, exampleMember1),
	// MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
	// MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
	MEMBER(MemberType::GAME_OBJECT_UID, blackImageObjUID)
};

GENERATE_BODY_IMPL(CanvasFader);

void CanvasFader::Start() {
	GameObject* blackImageObj = GameplaySystems::GetGameObject(blackImageObjUID);
	if (blackImageObj) {
		blackImage = blackImageObj->GetComponent<ComponentImage>();
	}
}

void CanvasFader::Update() {
	UpdateBlackImage();
}

void CanvasFader::FadeIn() {
	Play();
	fadeState = FadeState::FADE_IN;
}

void CanvasFader::FadeOut() {
	Play();
	fadeState = FadeState::FADE_OUT;
}

void CanvasFader::Stop() {
	fadeState = FadeState::IDLE;
	playing = false;
}

bool CanvasFader::IsPlaying() const {
	return playing && blackImage;
}

void CanvasFader::UpdateBlackImage() {
	switch (fadeState) {
	case FadeState::IDLE:
		break;
	case FadeState::FADE_IN:
		if (fadeTimer > fadeDuration) {
			blackImage->SetColor(noAlphaBlack);
			Stop();
		} else {
			blackImage->SetColor(float4::Lerp(alphaBlack, noAlphaBlack, Clamp01(fadeTimer / fadeDuration)));
		}
		fadeTimer += Time::GetDeltaTime();
		break;
	case FadeState::FADE_OUT:
		if (fadeTimer > fadeDuration) {
			blackImage->SetColor(alphaBlack);
			Stop();
		} else {
			blackImage->SetColor(float4::Lerp(noAlphaBlack, alphaBlack, Clamp01(fadeTimer / fadeDuration)));
		}
		fadeTimer += Time::GetDeltaTime();
		break;
	}
}

void CanvasFader::Play() {
	fadeTimer = 0.0f;
	playing = true;
}