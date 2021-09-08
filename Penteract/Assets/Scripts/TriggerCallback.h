#pragma once

#include "Scripting/Script.h"

class FactoryDoors;
class DialogueManager;

class TriggerCallback : public Script
{
	GENERATE_BODY(TriggerCallback);

public:
	void Start() override;
	void Update() override;
	void OpenFactoryDoors();

public:
	UID gameControllerUID = 0;

private:
	bool hasOpenedDialog = false;
	DialogueManager* dialogueManagerScript = nullptr;
	FactoryDoors* factoryDoorsScript = nullptr;
};

