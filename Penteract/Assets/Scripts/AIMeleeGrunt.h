#pragma once

#include "Scripting/Script.h"

#include "AIState.h"

class ComponentAnimation;
class ComponentTransform;
class ComponentAgent;

class HUDController;
class PlayerController;
class AIMovement;

class AIMeleeGrunt : public Script
{
	GENERATE_BODY(AIMeleeGrunt);

public:

	void Start() override;
	void Update() override;
	void OnAnimationFinished() override;
	void OnAnimationSecondaryFinished() override;
	void HitDetected(int damage_ = 1);

public:

	UID playerUID = 0;
	UID canvasUID = 0;

	GameObject* player = nullptr;
	ComponentAgent* agent = nullptr;

	int maxSpeed = 8;
	int fallingSpeed = 30;
	float searchRadius = 30.f;
	float meleeRange = 5.f;
	int lifePoints = 5;
	float timeToDie = 5.f;
	bool dead = false;

private:

	float3 velocity = float3(0, 0, 0);
	AIState state = AIState::START;
	bool hitTaken = false;
	ComponentAnimation* animation = nullptr;
	ComponentTransform* ownerTransform = nullptr;
	int damageRecieved = 0;

	HUDController* hudControllerScript = nullptr;
	PlayerController* playerController = nullptr;
	AIMovement* movementScript = nullptr;

};

