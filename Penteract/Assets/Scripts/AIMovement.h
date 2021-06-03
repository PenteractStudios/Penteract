#pragma once

#include "Enemy.h"
#include "Scripting/Script.h"

class ComponentAnimation;
class ComponentTransform;
class ComponentAgent;

class HUDController;

enum class AIState {
	START,
	SPAWN,
	IDLE,
	RUN,
	ATTACK,
	DEATH
};

class AIMovement : public Script
{
	GENERATE_BODY(AIMovement);

public:

	void Start() override;
	void Update() override;
	void OnAnimationFinished() override;
	void HitDetected(int damage_ = 1);

private:
	bool CharacterInSight(const GameObject* character);
	bool CharacterInMeleeRange(const GameObject* character);
	void Seek(const float3& newPosition, float speed);


public:

	UID playerUID = 0;
	UID canvasUID = 0;

	GameObject* player = nullptr;
	ComponentAgent* agent = nullptr;

	Enemy gruntCharacter = Enemy(5, 8.0f, 30, 40.f, 5.f, 5.f);

private:

	float3 velocity = float3(0, 0, 0);
	AIState state = AIState::START;
	bool hitTaken = false;
	ComponentAnimation* animation = nullptr;
	ComponentTransform* parentTransform = nullptr;
	int damageRecieved = 0;

	HUDController* hudControllerScript = nullptr;

};
