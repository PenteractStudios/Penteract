#pragma once

#include "Scripting/Script.h"

class GameObject;
class ComponentSelectable;
class ComponentAudioSource;

class StartButton : public Script
{
	GENERATE_BODY(StartButton);

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
	UID sceneUID = 0;
	int checkpointNum = -1;

private:
	GameObject* player = nullptr;
	bool playHoveredAudio = true;
	ComponentSelectable* selectable = nullptr;
	ComponentAudioSource* audios[static_cast<int>(AudioType::TOTAL)] = { nullptr };
};
