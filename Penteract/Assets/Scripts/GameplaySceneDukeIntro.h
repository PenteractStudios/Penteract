#pragma once

#include "Scripting/Script.h"
#include "AIState.h"

class GameObject;
class ComponentAgent;
class ComponentVideo;
class ComponentAnimation;
class AIMovement;

class GameplaySceneDukeIntro : public Script
{
	GENERATE_BODY(GameplaySceneDukeIntro);

public:

	void Start() override;
	void Update() override;

public:
	UID duke1UID = 0;
	UID playerUID = 0;
	UID encounterPlazaUID = 0;
	UID videoUID = 0;

	float3 dukeRunTowards = float3(-70.f, 3.f, 0.f);
	float dukeDisappearDistance = 45.f;
	
	float dukeTimeToGo = 5.f;

private:
	GameObject* duke1 = nullptr;
	GameObject* player = nullptr;
	GameObject* encounterPlaza = nullptr;

	ComponentAnimation* dukeAnimation = nullptr;
	ComponentAgent* dukeAgent = nullptr;
	ComponentVideo* videoIntro = nullptr;
	AIMovement* movementScript = nullptr;
	AIState state = AIState::RUN;

	float currentDukeTimeToWalkAway = 0.f;

	void Movement();
};

