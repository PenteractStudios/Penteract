#pragma once

#include "Scripting/Script.h"

class GameObject;
class ComponentAgent;

class GameplaySceneDukeIntro : public Script
{
	GENERATE_BODY(GameplaySceneDukeIntro);

public:

	void Start() override;
	void Update() override;

public:
	UID duke1UID = 0;
	UID encounterPlazaUID = 0;

private:
	GameObject* duke1 = nullptr;
	GameObject* encounterPlaza = nullptr;

	ComponentAgent* dukeAgent = nullptr;	//Reference to Agent component, for navigation
};

