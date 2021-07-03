#pragma once

#include "Scripting/Script.h"

class ComponentAudioSource;
class ComponentSelectable;

extern bool screenResolutionChangeConfirmationWasRequested;
extern unsigned preSelectedScreenResolutionPreset;

class ScreenResolutionConfirmer : public Script {
	GENERATE_BODY(ScreenResolutionConfirmer);

public:

	enum class AudioType {
		HOVERED,
		CLICKED,
		TOTAL
	};

	void Start() override;
	void Update() override;
	void OnButtonClick()override;
	void PlayAudio(AudioType type);

private:
	bool playHoveredAudio = true;
	ComponentSelectable* selectable = nullptr;
	ComponentAudioSource* audios[static_cast<int>(AudioType::TOTAL)] = { nullptr };
};

