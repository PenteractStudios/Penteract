#pragma once

#include "Scripting/Script.h"

class GameObject;

class UISpriteSheetPlayer : public Script {
	GENERATE_BODY(UISpriteSheetPlayer);

public:

	void Start() override;
	void Update() override;

public:

	float secondsPerFrame = 0.1f;

private:
	float animationTimer = 0.0f;
	std::vector<GameObject*>frames;
	int currentFrame = 0;
	GameObject* currentFrameObj = nullptr;
};

