#pragma once

#include "Scripting/Script.h"

#include "AIState.h"
#include "Enemy.h"

class GameObject;
class ComponentAnimation;
class ComponentTransform;
class ComponentAgent;
class ComponentAudioSource;
class ComponentMeshRenderer;
class ResourcePrefab;
class HUDController;
class PlayerController;
class PlayerDeath;
class AIMovement;
class WinLose;
class EnemySpawnPoint;

class AIMeleeGrunt : public Script {
	GENERATE_BODY(AIMeleeGrunt);

public:
	enum class AudioType {
		SPAWN,
		ATTACK,
		FOOTSTEP_RIGHT,
		FOOTSTEP_LEFT,
		HIT,
		DEATH,
		TOTAL
	};

	void Start() override;
	void Update() override;
	void OnAnimationFinished() override;
	void OnAnimationEvent(StateMachineEnum stateMachineEnum, const char* eventName) override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;

	void EnableBlastPushBack();
	void DisableBlastPushBack();
	bool IsBeingPushed() const;
	void PlayerHit();
	void PlayHit();

public:

	UID playerUID = 0;
	UID canvasUID = 0;
	UID winConditionUID = 0;
	UID meleePunchUID = 0;
	UID fangUID = 0;
	// Hit feedback
	UID defaultMaterialPlaceHolderUID = 0;
	UID damageMaterialPlaceHolderUID = 0;
	UID defaultMaterialID = 0;
	UID damageMaterialID = 0;

	UID rightBladeColliderUID = 0;
	UID leftBladeColliderUID = 0;
	

	GameObject* player = nullptr;
	GameObject* fang = nullptr;
	GameObject* spawn = nullptr;
	ComponentAgent* agent = nullptr;
	WinLose* winLoseScript = nullptr;

	Enemy gruntCharacter = Enemy(5.0f, 8.0f, 1.0f, 30, 40.f, 5.f, 5.f, 5.f, 5.f, 3.f, 2.f);
	bool killSent = false;

	float hurtFeedbackTimeDuration = 0.5f;

	float stunDuration = 3.f;

	float groundPosition = 3.0f;

private:

	float attackDuration = 2.2f;
	float attackRemaining = 0.0f;
	bool attackRightColliderOn = false;
	bool attackLeftColliderOn = false;
	bool track = true;
	bool attackStep = false;
	bool alreadyHit = false;
	int attackNumber = 3;

	float3 velocity = float3(0, 0, 0);
	AIState state = AIState::START;
	bool hitTaken = false;
	ComponentAnimation* animation = nullptr;
	ComponentTransform* ownerTransform = nullptr;
	int damageRecieved = 0;

	float stunTimeRemaining = 0.f;

	//bool EMPUpgraded = false;
	int deathType = 0;

	HUDController* hudControllerScript = nullptr;
	PlayerController* playerController = nullptr;
	PlayerDeath* playerDeath = nullptr;
	AIMovement* movementScript = nullptr;
	EnemySpawnPoint* enemySpawnPointScript = nullptr;

	ComponentAudioSource* audios[static_cast<int>(AudioType::TOTAL)] = { nullptr };
	ComponentMeshRenderer* componentMeshRenderer = nullptr;

	float timeSinceLastHurt = 0.5f;
	GameObject* rightBladeCollider = nullptr;
	GameObject* leftBladeCollider = nullptr;

	float currentPushBackDistance = 0.f;
	float currentSlowedDownTime = 0.f;

	float pushBackRealDistance = 0.f;

private:
	void UpdatePushBackPosition();
	void CalculatePushBackRealDistance();	// Calculates the real distance of the pushback taking into account any obstacles in the path
	void Death();
};
