#pragma once

#include "Scripting/Script.h"

class PlayerDeath : public Script
{
	GENERATE_BODY(PlayerDeath);

public:

	void Start() override;
	void Update() override;
	void OnAnimationFinished() override;

public:

	UID playerUID = 0;
	UID sceneUID = 0;

	GameObject* player = nullptr;

	bool dead = false;

};

