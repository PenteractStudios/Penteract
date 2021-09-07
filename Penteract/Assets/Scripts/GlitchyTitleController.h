#pragma once

#include "Scripting/Script.h"

class ComponentImage;
class UISpriteSheetPlayer;
class CanvasFader;

class GlitchyTitleController : public Script {
	GENERATE_BODY(GlitchyTitleController);

public:

	void Start() override;
	void Update() override;
	void PressedPlay(); //Method to be called to trigger FadeOut when play is pressed and whiel fade to black is happening
	bool ReadyForTransition()const;

public:
	float maxTimeForGlitch = 2.0f;
	float minTimeForGlitch = 4.0f;
	float fadeInTotalTime = 0.5f;
	UID startUpTitleObjUID = 0;
	UID loopingTitleObjUID = 0;
	UID glitchTitleObjUID = 0;
	UID startPlayingTitleObjUID = 0;
	UID blackImageObjUID = 0;


private:
	void StartStateIdle();

private:
	enum class GlitchState { FADE_IN, WAIT_START, START, IDLE, GLITCH, PLAY, WAIT_FADE_OUT, FADE_OUT };
	GlitchState glitchState = GlitchState::FADE_IN;
	float glitchTimer = 10.0f;
	float fadeInTimer = 0.0f;

	UISpriteSheetPlayer* startUpTitleSpriteSheetPlayer = nullptr;
	UISpriteSheetPlayer* loopingTitleSpriteSheetPlayer = nullptr;
	UISpriteSheetPlayer* glitchTitleSpriteSheetPlayer = nullptr;
	UISpriteSheetPlayer* startPlayingTitleSpriteSheetPlayer = nullptr;
	CanvasFader* canvasFader = nullptr;
	const float4 alphaBlack = float4(0, 0, 0, 1);
	const float4 noAlphaBlack = float4(0, 0, 0, 0);



};

