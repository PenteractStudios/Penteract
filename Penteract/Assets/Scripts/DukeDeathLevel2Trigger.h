#pragma once

#include "Scripting/Script.h"

class GameObject;
class ComponentMeshRenderer;
class DialogueManager;
class AfterDialogCallback;
class MovingLasers;
class AIDuke;

class DukeDeathLevel2Trigger : public Script
{
	GENERATE_BODY(DukeDeathLevel2Trigger);

public:
	void Start() override;
	void Update() override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;

	void PlayDissolveAnimation(GameObject* root, bool playReverse);

public:
	UID dukeUID = 0;
	UID gameControllerUID = 0;
	UID laserUID = 0;
	UID HUDUID = 0;
	
	int dialogueID = 0;

	UID entranceDoorUID = 0;
	UID exitDoorUID = 0;

	// Dissolve effects
	UID doorDissolveUID = 0;

private:
	GameObject* duke = nullptr;
	GameObject* gameController = nullptr;
	ComponentMeshRenderer* dukeMaterial = nullptr;
	DialogueManager* dialogueManagerScript = nullptr;
	MovingLasers* laserScript = nullptr;
	AIDuke* aiDuke = nullptr;

	GameObject* entraceDoor = nullptr;
	GameObject* exitDoor = nullptr;

	bool triggered = false;

	/* Dissolve UID */
	UID doorDissolveMaterial = 0;
};

