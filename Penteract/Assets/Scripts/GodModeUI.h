#pragma once

#include "Scripting/Script.h"

class GameObject;

class GodModeUI : public Script
{
	GENERATE_BODY(GodModeUI);

public:

	void Start() override;
	void Update() override;

private:
	GameObject* gameObject = nullptr;
	std::vector<GameObject*> children;
};

