#pragma once

#include "Scripting/Script.h"

class GameObject;
class DialogueManager;
class AfterDialogCallback;
class MovingLasers;

class TriggerDialogueBossLvl2Start : public Script {
	GENERATE_BODY(TriggerDialogueBossLvl2Start);

public:
	void Start() override;
	void Update() override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;

public:
	UID BossUID = 0;
	UID gameControllerUID = 0;
	UID afterDialogCallbackUID = 0;
	UID laserUID = 0;
	bool SwitchOn = true;
	GameObject* boss = nullptr;
	GameObject* gameController = nullptr;
	DialogueManager* dialogueManagerScript = nullptr;
	int dialogueID = 0;

private:
	AfterDialogCallback* afterDialogCallbackScript = nullptr;
	MovingLasers* laserScript = nullptr;

	bool triggered = false; // Two objects can collide with the trigger at the same time. This boolean prevents the OnCollision() code to be executed twice when that happens
};
