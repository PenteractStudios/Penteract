#pragma once

#include "Scripting/Script.h"

class GameObject;
class DialogueManager;
class PlayerController;
class TriggerPowerUp : public Script
{
	GENERATE_BODY(TriggerPowerUp);

public:

	void Start() override;
	void Update() override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;

public:
	UID gameControllerUID = 0;
	UID playerUID = 0;
	GameObject* gameController = nullptr;
	GameObject* player = nullptr;
	DialogueManager* dialogueManagerScript = nullptr;
	PlayerController* playerControllerScript = nullptr;

};

