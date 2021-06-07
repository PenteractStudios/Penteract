#pragma once

#include "Scripting/Script.h"
#include "AIMovement.h"

#include <string>

class ComponentAgent;
class ComponentAudioSource;
class ComponentAnimation;
class ComponentMeshRenderer;
class ResourcePrefab;
class HUDController;
class PlayerController;

enum class RangeAIState {
	START,
	SPAWN,
	IDLE,
	APPROACH,
	SHOOT,
	FLEE,
	DEATH
};

class RangedAI : public Script {
	GENERATE_BODY(RangedAI);

public:

	void Start() override;
	void Update() override;
	void OnAnimationFinished() override;
	void OnAnimationSecondaryFinished() override;
	void HitDetected(int damage_ = 1);
	void ShootPlayerInRange();
private:
	//State handling
	void EnterState(RangeAIState newState);
	void UpdateState();
	void ExitState();
	void ChangeState(RangeAIState newState);

	bool CharacterInSight(const GameObject* character);
	bool CharacterInRange(const GameObject* character, float range, bool useRange);
	bool FindsRayToCharacter(const GameObject* character, bool useForward);
	bool CharacterTooClose(const GameObject* character);
	void Seek(const float3& newPosition, int speed);
	void Flee(const float3& fromPosition, int speed);
	void StopMovement();
	void OrientateTo(const float3& direction);
	std::string StateToString(RangeAIState state);
	void ActualShot();
public:
	bool foundRayToPlayer = false;
	UID playerUID = 0;
	UID playerMeshUIDFang = 0;
	UID playerMeshUIDOnimaru = 0;
	UID meshUID = 0;
	UID meshUID1 = 0;
	UID meshUID2 = 0;
	UID agentObjectUID = 0;
	UID trailPrefabUID = 0;
	UID hudControllerObjUID = 0;


	ResourcePrefab* shootTrailPrefab = nullptr;
	GameObject* player = nullptr;

	HUDController* hudControllerScript = nullptr;
	PlayerController* playerController = nullptr;

	GameObject* fangMeshObj = nullptr;
	GameObject* onimaruMeshObj = nullptr;

	GameObject* meshObj = nullptr;
	GameObject* meshObj1 = nullptr;
	GameObject* meshObj2 = nullptr;

	UID dmgMaterialObj = 0;
	UID noDmgMaterialID = 0;
	UID damagedMaterialID = 0;


	int maxMovementSpeed = 8;
	int fallingSpeed = 30;

	float searchRadius = 40.f;

	int lifePoints = 5;
	float timeToDie = 5.f;

	bool dead = false;

	float attackRange = 20.0f;
	float approachOffset = 4.0f;

	float fleeingRange = 7.f;
	float fleeingEvaluateDistance = 5.0f;


	float attackSpeed = 0.5f; //Shots per second
	ComponentAgent* agent = nullptr;
	float actualShotMaxTime = 0.3f;
	float timeSinceLastHurt = 0.5f;
private:
	float3 bbCenter = float3(0, 0, 0);
	float3 velocity = float3(0, 0, 0);
	RangeAIState state = RangeAIState::START;
	bool hitTaken = false;
	bool shot;
	ComponentAnimation* animation = nullptr;
	ComponentTransform* parentTransform = nullptr;
	int damageRecieved = 0;
	float attackTimePool = 2.0f;
	float actualShotTimer = -1.0f;

	float hurtFeedbackTimeDuration = 0.5f;

	ComponentMeshRenderer* meshRenderer = nullptr;
	ComponentAudioSource* shootAudioSource = nullptr;
};

