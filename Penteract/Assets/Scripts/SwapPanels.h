#pragma once

#include "Scripting/Script.h"

#include "GameObject.h"

class ComponentAudioSource;
class ComponentSelectable;

class SwapPanels : public Script
{
	GENERATE_BODY(SwapPanels);

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
	UID targetUID;
	UID currentUID;
	GameObject* target;
	GameObject* current;

private:
	bool playHoveredAudio = true;
	ComponentSelectable* selectable = nullptr;
	ComponentAudioSource* audios[static_cast<int>(AudioType::TOTAL)] = { nullptr };
};

