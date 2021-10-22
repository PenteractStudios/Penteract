#pragma once

#include "Scripting/Script.h"

class GameObject;

class CheckpointButtonsController : public Script
{
	GENERATE_BODY(CheckpointButtonsController);

public:

	void Start() override;
	void Update() override;


public:
	UID btnLevel1_Checkpoint0UID, btnLevel1_Checkpoint1UID, btnLevel1_Checkpoint2UID, btnLevel1_Checkpoint3UID, btnLevel1_Checkpoint4UID;
	UID btnLevel2_Checkpoint0UID, btnLevel2_Checkpoint1UID, btnLevel2_Checkpoint2UID, btnLevel2_Checkpoint3UID;
	UID btnLevel3_Checkpoint0UID;

	/* Buttons */
	GameObject* btnLevel1_Checkpoint0 = nullptr;
	GameObject* btnLevel1_Checkpoint1 = nullptr;
	GameObject* btnLevel1_Checkpoint2 = nullptr;
	GameObject* btnLevel1_Checkpoint3 = nullptr;
	GameObject* btnLevel1_Checkpoint4 = nullptr;
	GameObject* btnLevel2_Checkpoint0 = nullptr;
	GameObject* btnLevel2_Checkpoint1 = nullptr;
	GameObject* btnLevel2_Checkpoint2 = nullptr;
	GameObject* btnLevel2_Checkpoint3 = nullptr;
	GameObject* btnLevel3_Checkpoint0 = nullptr;

	std::vector<GameObject*> buttonsLevel1;
	std::vector<GameObject*> buttonsLevel2;

	int actualLevel = 1; 
	int actualCheckpoint = 0;
};

