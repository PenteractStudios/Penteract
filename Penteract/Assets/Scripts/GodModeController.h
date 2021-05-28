#pragma once

#include "Scripting/Script.h"
#include <vector>

class GameObject;
class ComponentToggle;

class GodModeController : public Script {
	GENERATE_BODY(GodModeController);

public:
	UID uiCanvasUID = 0;

public:
	void Start() override;
	void Update() override;
	void OnChildToggle(unsigned int index, bool isChecked);

private:
	GameObject* gameObject = nullptr;
	GameObject* uiCanvas = nullptr;

	std::vector<GameObject*> children;
	std::vector<ComponentToggle*> toggles;
};
