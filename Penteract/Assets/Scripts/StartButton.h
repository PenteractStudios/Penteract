#pragma once

#include "Scripting/Script.h"

class GameObject;
class SceneTransition;

class StartButton : public Script
{
	GENERATE_BODY(StartButton);

public:

	void Start() override;
	void Update() override;
	void OnButtonClick() override;

public:
	UID sceneUID = 0;
	UID transitionUID = 0;

	int checkpointNum = -1;
	bool isClickedButton = false;

private:
	GameObject* player = nullptr;
	GameObject* transitionGO = nullptr;

	SceneTransition* sceneTransition = nullptr;
};
