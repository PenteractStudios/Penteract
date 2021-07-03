#pragma once

#include "Scripting/Script.h"

class ComponentAudioSource;
class ComponentSelectable;

class ExitButton : public Script
{
	GENERATE_BODY(ExitButton);

public:

	enum class AudioType {
		HOVERED,
		CLICKED,
		TOTAL
	};

	void Start() override;
	void Update() override;
	void OnButtonClick() override;
	void PlayAudio(AudioType type);

private:
	bool playHoveredAudio = true;
	ComponentSelectable* selectable = nullptr;
	ComponentAudioSource* audios[static_cast<int>(AudioType::TOTAL)] = { nullptr };
};
