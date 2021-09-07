#include "GlitchyTitleController.h"

#include "GameplaySystems.h"
#include "UISpriteSheetPlayer.h"
#include "Components/UI/ComponentImage.h"
#include "CanvasFader.h"

EXPOSE_MEMBERS(GlitchyTitleController) {
	MEMBER(MemberType::FLOAT, minTimeForGlitch),
	MEMBER(MemberType::FLOAT, maxTimeForGlitch),
	MEMBER(MemberType::GAME_OBJECT_UID, startUpTitleObjUID),
	MEMBER(MemberType::GAME_OBJECT_UID, loopingTitleObjUID),
	MEMBER(MemberType::GAME_OBJECT_UID, glitchTitleObjUID),
	MEMBER(MemberType::GAME_OBJECT_UID, startPlayingTitleObjUID),
	MEMBER(MemberType::GAME_OBJECT_UID, blackImageObjUID)
};

GENERATE_BODY_IMPL(GlitchyTitleController);

void GlitchyTitleController::Start() {
	GameObject* startUpTitleObj = GameplaySystems::GetGameObject(startUpTitleObjUID);
	GameObject* loopingTitleObj = GameplaySystems::GetGameObject(loopingTitleObjUID);
	GameObject* glitchTitleObj = GameplaySystems::GetGameObject(glitchTitleObjUID);
	GameObject* startPlayingTitleObj = GameplaySystems::GetGameObject(startPlayingTitleObjUID);
	GameObject* blackImageObj = GameplaySystems::GetGameObject(blackImageObjUID);

	if (blackImageObjUID) {
		canvasFader = GET_SCRIPT(blackImageObj, CanvasFader);
		if (canvasFader) {
			//blackImage->SetColor(alphaBlack);
			canvasFader->FadeIn();
		}
	}

	if (startUpTitleObj) {
		startUpTitleSpriteSheetPlayer = GET_SCRIPT(startUpTitleObj, UISpriteSheetPlayer);
	}

	if (loopingTitleObj) {
		loopingTitleSpriteSheetPlayer = GET_SCRIPT(loopingTitleObj, UISpriteSheetPlayer);
	}

	if (glitchTitleObj) {
		glitchTitleSpriteSheetPlayer = GET_SCRIPT(glitchTitleObj, UISpriteSheetPlayer);
	}

	if (startPlayingTitleObj) {
		startPlayingTitleSpriteSheetPlayer = GET_SCRIPT(startPlayingTitleObj, UISpriteSheetPlayer);
	}

}

void GlitchyTitleController::Update() {

	if (!startUpTitleSpriteSheetPlayer || !loopingTitleSpriteSheetPlayer || !glitchTitleSpriteSheetPlayer || !startPlayingTitleSpriteSheetPlayer)return;
	if (startUpTitleSpriteSheetPlayer->IsPlaying() || startPlayingTitleSpriteSheetPlayer->IsPlaying())return;

	switch (glitchState) {
	case GlitchState::FADE_IN:
		if (canvasFader) {
			//fadeInTimer += Time::GetDeltaTime();
			//blackImage->SetColor(float4::Lerp(alphaBlack, noAlphaBlack, Clamp01(fadeInTimer / fadeInTotalTime)));

			//if (fadeInTimer >= fadeInTotalTime) {
			//	glitchState = GlitchState::WAIT_START;
			//	fadeInTimer = 0;
			//}

			if (!canvasFader->IsPlaying()) {
				glitchState = GlitchState::WAIT_START;
			}

		} else {
			//Error prevention
			glitchState = GlitchState::WAIT_START;
		}
		break;
	case GlitchState::WAIT_START:
		fadeInTimer += Time::GetDeltaTime();
		if (fadeInTimer >= fadeInTotalTime) {
			glitchState = GlitchState::START;
			startUpTitleSpriteSheetPlayer->Play();
		}
		break;
	case GlitchState::START:
		StartStateIdle();
		break;
	case GlitchState::IDLE:
		if (glitchTimer <= 0) {
			loopingTitleSpriteSheetPlayer->Stop();
			glitchTitleSpriteSheetPlayer->Play();
			glitchState = GlitchState::GLITCH;
		}
		glitchTimer -= Time::GetDeltaTime();
		break;
	case GlitchState::GLITCH:
		if (glitchTitleSpriteSheetPlayer->IsPlaying())return;
		StartStateIdle();
		break;
	case GlitchState::PLAY:
		fadeInTimer = 0.0f;
		glitchState = GlitchState::WAIT_FADE_OUT;
		break;
	case GlitchState::WAIT_FADE_OUT:
		fadeInTimer += Time::GetDeltaTime();
		if (fadeInTimer >= fadeInTotalTime) {
			canvasFader->FadeOut();
			glitchState = GlitchState::FADE_OUT;
		}
		break;
	case GlitchState::FADE_OUT:
		break;
	}
}

void GlitchyTitleController::PressedPlay() {
	glitchState = GlitchState::PLAY;
	loopingTitleSpriteSheetPlayer->Stop();
	glitchTitleSpriteSheetPlayer->Stop();
	startPlayingTitleSpriteSheetPlayer->Stop();
	startPlayingTitleSpriteSheetPlayer->Play();
}

bool GlitchyTitleController::ReadyForTransition() const {
	return glitchState == GlitchState::FADE_OUT;
}

void GlitchyTitleController::StartStateIdle() {
	glitchState = GlitchState::IDLE;
	glitchTimer = minTimeForGlitch + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxTimeForGlitch - minTimeForGlitch)));
	loopingTitleSpriteSheetPlayer->Play();
}
