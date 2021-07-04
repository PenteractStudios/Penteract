#pragma once

#include "Scripting/Script.h"
#include "UIAudioType.h"

class ComponentAudioSource;
class ComponentSelectable;

class ExitButton : public Script
{
	GENERATE_BODY(ExitButton);

public:

	void Start() override;
	void Update() override;
	void OnButtonClick() override;
	void PlayAudio(UIAudio type);

private:
	bool playHoveredAudio = true;
	ComponentSelectable* selectable = nullptr;
	ComponentAudioSource* audios[static_cast<int>(UIAudio::TOTAL)] = { nullptr };
};
