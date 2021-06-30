#pragma once

#include "Scripting/Script.h"

class GameObject;
class ComponentTransform2D;

class SceneTransition : public Script
{
	GENERATE_BODY(SceneTransition);

public:

	void Start() override;
	void Update() override;

public:

	UID sceneUID = 0;
	UID transitionUID = 0;

	int speedTransition = 10;
	bool finishedTransition = false;

private:
	GameObject* transitionGO = nullptr;

	ComponentTransform2D* transform2D = nullptr;
};

