#pragma once

#include "Scripting/Script.h"
#include "Enemy.h"
#include "AIState.h"


#include <string>

class ComponentAgent;
class ComponentAudioSource;
class ComponentAnimation;
class ComponentMeshRenderer;
class ComponentTransform;
class ResourcePrefab;
class HUDController;
class PlayerController;
class AIMovement;

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
	void EnterState(AIState newState);
	void UpdateState();
	void ExitState();
	void ChangeState(AIState newState);

	bool CharacterInSight(const GameObject* character);
	bool CharacterInRange(const GameObject* character, float range, bool useRange);
	bool FindsRayToPlayer(bool useForward);
	bool CharacterTooClose(const GameObject* character);
	void OrientateTo(const float3& direction);
	void ActualShot();
public:

	Enemy rangerGruntCharacter = Enemy(5, 8.0f, 1, 30, 40.f, 5.f, 5.f);

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
	AIMovement* aiMovement = nullptr;
	float3 bbCenter = float3(0, 0, 0);
	float3 velocity = float3(0, 0, 0);
	AIState state = AIState::START;
	bool hitTaken = false;
	bool shot = false;
	ComponentAnimation* animation = nullptr;
	ComponentTransform* parentTransform = nullptr;
	int damageRecieved = 0;
	float attackTimePool = 2.0f;
	float actualShotTimer = -1.0f;

	float hurtFeedbackTimeDuration = 0.5f;

	ComponentMeshRenderer* meshRenderer = nullptr;
	ComponentAudioSource* shootAudioSource = nullptr;
};

