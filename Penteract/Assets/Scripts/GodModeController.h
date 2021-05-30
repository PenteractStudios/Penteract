#pragma once

#include "Scripting/Script.h"
#include <vector>

class GameObject;
class ComponentToggle;

class GodModeController : public Script {
	GENERATE_BODY(GodModeController);

public:
	UID uiCanvasUID = 0;
	UID spawnMeleeUID = 0;
	UID spawnRangedUID = 0;
	UID toggleEnemiesUID = 0;
	UID invincibleUID = 0;

public:
	void Start() override;
	void Update() override;
	void OnChildToggle(unsigned int index, bool isChecked);

private:
	GameObject* gameObject = nullptr;
	GameObject* uiCanvas = nullptr;
	GameObject* spawnMelee = nullptr;
	GameObject* spawnRanged = nullptr;
	GameObject* toggleEnemies = nullptr;
	GameObject* invincible = nullptr;

	std::vector<ComponentToggle*> toggles;
};
