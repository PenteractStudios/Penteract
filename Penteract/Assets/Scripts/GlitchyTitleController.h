#pragma once

#include "Scripting/Script.h"

class UISpriteSheetPlayer;

class GlitchyTitleController : public Script {
	GENERATE_BODY(GlitchyTitleController);

public:

	void Start() override;
	void Update() override;
	void PressedPlay(); //Method to be called to trigger FadeOut when play is pressed and whiel fade to black is happening

public:
	float maxTimeForGlitch = 2.0f;
	float minTimeForGlitch = 4.0f;

	UID startUpTitleObjUID = 0;
	UID loopingTitleObjUID = 0;
	UID glitchTitleObjUID = 0;
	UID startPlayingTitleObjUID = 0;

private:
	enum class GlitchState { START, IDLE, GLITCH, PLAY,FADE_OUT };
	GlitchState glitchState = GlitchState::START;
	float glitchTimer = 10.0f;

	UISpriteSheetPlayer* startUpTitleSpriteSheetPlayer = nullptr;
	UISpriteSheetPlayer* loopingTitleSpriteSheetPlayer = nullptr;
	UISpriteSheetPlayer* glitchTitleSpriteSheetPlayer = nullptr;
	UISpriteSheetPlayer* startPlayingTitleSpriteSheetPlayer = nullptr;

private:
	void StartStateIdle();
};

