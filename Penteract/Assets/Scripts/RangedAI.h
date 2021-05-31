#pragma once

#include "Scripting/Script.h"
#include "AIMovement.h"

#include <string>

class ComponentAgent;
class ComponentAudioSource;

enum class RangeAIState {
	START,
	SPAWN,
	IDLE,
	APPROACH,
	SHOOT,
	FLEE,
	HURT,
	DEATH
};

class RangedAI : public Script {
	GENERATE_BODY(RangedAI);

public:

	void Start() override;
	void Update() override;
	void OnAnimationFinished() override;
	void HitDetected(int damage_ = 1);
	void ShootPlayerInRange();
private:
	//State handling
	void EnterState(RangeAIState newState);
	void UpdateState();
	void ExitState();
	void ChangeState(RangeAIState newState);

	bool CharacterInSight(const GameObject* character);
	bool CharacterInRange(const GameObject* character);
	bool CharacterShootable(const GameObject* character, bool useForward);
	bool CharacterTooClose(const GameObject* character);
	void Seek(const float3& newPosition, int speed);
	void Flee(const float3& fromPosition, int speed);
	void StopMovement();
	void OrientateTo(const float3& direction);
	std::string StateToString(RangeAIState state);
public:
	bool foundRayToPlayer = false;
	UID playerUID = 0;
	UID playerMeshUIDFang = 0;
	UID playerMeshUIDOnimaru = 0;
	UID meshUID = 0;
	UID meshUID1 = 0;
	UID meshUID2 = 0;
	UID agentObjectUID = 0;
	GameObject* player = nullptr;
	GameObject* meshObj = nullptr;
	GameObject* meshObj1 = nullptr;
	GameObject* meshObj2 = nullptr;

	int maxMovementSpeed = 8;
	int fallingSpeed = 30;

	float searchRadius = 40.f;

	int lifePoints = 5;
	float timeToDie = 5.f;

	float maxStunnedTime = 0.5f;
	bool dead = false;

	float attackRange = 18;

	float fleeingRange = 10.f;
	float fleeingEvaluateDistance = 5.0f;


	float attackSpeed = 0.5f; //Shots per second
	ComponentAgent* agent = nullptr;

private:
	float3 bbCenter = float3(0, 0, 0);
	float3 velocity = float3(0, 0, 0);
	RangeAIState state = RangeAIState::START;
	bool hitTaken = false;
	ComponentAnimation* animation = nullptr;
	ComponentTransform* parentTransform = nullptr;
	int damageRecieved = 0;
	float timeStunned = 0.0f;
	float attackTimePool = 0.0f;

	ComponentAudioSource* shootAudioSource = nullptr;
};

