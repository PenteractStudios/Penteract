#pragma once

#include "Scripting/Script.h"

class GameObject;
class ComponentTransform2D;
class ComponentImage;

class SceneTransition : public Script
{
	GENERATE_BODY(SceneTransition);

public:
	enum class TransitionMove {
		NONE,
		LEFT_TO_RIGHT,
		RIGHT_TO_LEFT,
		TOP_TO_BOTTOM,
		BOTTOM_TO_TOP,
	};

	void Start() override;
	void Update() override;

	void StartTransition(bool isExit_ = false);

public:

	UID sceneUID = 0;
	UID transitionUID = 0;

	int transitionMove = (int) TransitionMove::LEFT_TO_RIGHT;
	float speedTransition = 20;

private:
	GameObject* transitionGO = nullptr;
	ComponentTransform2D* transform2D = nullptr;
	ComponentImage* image2D = nullptr;

	bool startTransition = false;
	bool finishedTransition = false;
	bool isExit = false;
};

