#pragma once

#include "Scripting/Script.h"

class ComponentAudioSource;
class ComponentSelectable;

class Resume : public Script
{
	GENERATE_BODY(Resume);

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

public:
	UID pauseUID;
	UID hudUID;

private:
	GameObject* pauseCanvas = nullptr;
	GameObject* hudCanvas = nullptr;

	bool playHoveredAudio = true;
	ComponentSelectable* selectable = nullptr;
	ComponentAudioSource* audios[static_cast<int>(AudioType::TOTAL)] = { nullptr };
};

