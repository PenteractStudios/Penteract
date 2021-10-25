#pragma once

#include "Scripting/Script.h"
#include "AIState.h"

class GameObject;
class ComponentAgent;
class ComponentVideo;
class ComponentAnimation;
class ComponentTransform;
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
	float dukeSpeed = 3.f;
	float dukeDisappearDistance = 45.f;
	
	float dukeTimeToChange = 5.f;
	float rotationAngle = 77.783f;

private:
	GameObject* duke1 = nullptr;
	GameObject* player = nullptr;
	GameObject* encounterPlaza = nullptr;

	ComponentAnimation* dukeAnimation = nullptr;
	ComponentAgent* dukeAgent = nullptr;
	ComponentVideo* videoIntro = nullptr;
	ComponentTransform* dukeTransform = nullptr;
	AIMovement* movementScript = nullptr;
	AIState state = AIState::RUN;

	float currentDukeTimeToChange = 0.f;
	bool moveBackward = true;

	void Movement();
};

