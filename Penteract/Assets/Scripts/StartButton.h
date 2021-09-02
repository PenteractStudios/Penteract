#pragma once

#include "Scripting/Script.h"
#include "UIAudioType.h"

class GameObject;
class ComponentSelectable;
class ComponentAudioSource;
class ComponentImage;
class SceneTransition;

class StartButton : public Script {
	GENERATE_BODY(StartButton);

public:

	void Start() override;
	void Update() override;
	void OnButtonClick() override;
	void PlayAudio(UIAudio type);

public:
	UID sceneUID = 0;
	UID transitionUID = 0;
	UID fadeToBlackObjectUID = 0;
	int checkpointNum = -1;
	int levelNum = 0;
	float fadeDuration = 3.0f;
private:
	GameObject* player = nullptr;

	GameObject* transitionGO = nullptr;
	SceneTransition* sceneTransition = nullptr;

	/* UI */
	ComponentImage* fadeToBlackImage = nullptr;
	float fadeTimer = 0.0f;

	/* Audio */
	bool playHoveredAudio = true;
	bool pressed = false;
	ComponentSelectable* selectable = nullptr;
	ComponentAudioSource* audios[static_cast<int>(UIAudio::TOTAL)] = { nullptr };

private:
	void DoTransition();

};
