#pragma once

#include "Scripting/Script.h"

#include "AIState.h"
#include "Enemy.h"

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
	void OnCollision(const GameObject& collidedWith) override;

public:

	UID playerUID = 0;
	UID canvasUID = 0;

	GameObject* player = nullptr;
	ComponentAgent* agent = nullptr;

	Enemy gruntCharacter = Enemy(5, 8.0f, 1, 30, 40.f, 5.f, 5.f);

private:

	float3 velocity = float3(0, 0, 0);
	AIState state = AIState::START;
	ComponentAnimation* animation = nullptr;
	ComponentTransform* ownerTransform = nullptr;

	HUDController* hudControllerScript = nullptr;
	PlayerController* playerController = nullptr;
	AIMovement* movementScript = nullptr;

};

