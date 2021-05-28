#pragma once

#include "Scripting/Script.h"
#include <vector>

class GameObject;

class GodModeController : public Script {
	GENERATE_BODY(GodModeController);

public:
	void Start() override;
	void Update() override;
	void OnChildToggle(unsigned int index);

private:
	GameObject* gameObject = nullptr;
	std::vector<GameObject*> children;

};
