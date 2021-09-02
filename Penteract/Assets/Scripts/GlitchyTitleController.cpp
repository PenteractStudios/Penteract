#include "GlitchyTitleController.h"
#include "GameplaySystems.h"
#include "UISpriteSheetPlayer.h"

EXPOSE_MEMBERS(GlitchyTitleController) {
	MEMBER(MemberType::FLOAT, minTimeForGlitch),
	MEMBER(MemberType::FLOAT, maxTimeForGlitch),
	MEMBER(MemberType::GAME_OBJECT_UID, startUpTitleObjUID),
	MEMBER(MemberType::GAME_OBJECT_UID, loopingTitleObjUID),
	MEMBER(MemberType::GAME_OBJECT_UID, glitchTitleObjUID),
	MEMBER(MemberType::GAME_OBJECT_UID, startPlayingTitleObjUID)
};

GENERATE_BODY_IMPL(GlitchyTitleController);

void GlitchyTitleController::Start() {
	GameObject* startUpTitleObj = GameplaySystems::GetGameObject(startUpTitleObjUID);
	GameObject* loopingTitleObj = GameplaySystems::GetGameObject(loopingTitleObjUID);
	GameObject* glitchTitleObj = GameplaySystems::GetGameObject(glitchTitleObjUID);
	GameObject* startPlayingTitleObj = GameplaySystems::GetGameObject(startPlayingTitleObjUID);

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
		glitchState = GlitchState::FADE_OUT;
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

void GlitchyTitleController::StartStateIdle() {
	glitchState = GlitchState::IDLE;
	glitchTimer = minTimeForGlitch + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxTimeForGlitch - minTimeForGlitch)));
	loopingTitleSpriteSheetPlayer->Play();
}
