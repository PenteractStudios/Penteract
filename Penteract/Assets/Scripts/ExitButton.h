#pragma once

#include "Scripting/Script.h"

class ComponentSelectable;

class GameObject;
class SceneTransition;

class ExitButton : public Script
{
	GENERATE_BODY(ExitButton);

public:

	void Start() override;
	void Update() override;
	void OnButtonClick() override;

public:
	UID transitionUID = 0;

private:
	GameObject* transitionGO = nullptr;
	SceneTransition* sceneTransition = nullptr;
	ComponentSelectable* selectable = nullptr;
};
