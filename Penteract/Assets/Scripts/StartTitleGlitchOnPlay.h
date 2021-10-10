#pragma once

#include "Scripting/Script.h"
#include "UIAudioType.h"

class GlitchyTitleController;
class GameObject;
class ComponentSelectable;
class ComponentAudioSource;

class StartTitleGlitchOnPlay : public Script {
	GENERATE_BODY(StartTitleGlitchOnPlay);

public:
	void Start() override;
	void Update() override;

	void OnButtonClick() override;
	void DoTransition();

public:
	UID controllerObjUID = 0;
	UID sceneUID = 0;
	UID parentCanvasUID = 0;

private:
	void PlayAudio(UIAudio type);

private:
	
	/* UI */
	GameObject* parentCanvas = nullptr;
	GlitchyTitleController* controller = nullptr;

	/* Audio */
	bool playHoveredAudio = true;
	ComponentSelectable* selectable = nullptr;
	ComponentAudioSource* audios[static_cast<int>(UIAudio::TOTAL)] = { nullptr };
};

