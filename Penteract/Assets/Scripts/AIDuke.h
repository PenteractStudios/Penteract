#pragma once

#include "Scripting/Script.h"
#include "Duke.h"

class GameObject;
class ComponentAnimation;
class ComponentTransform;
class ComponentAgent;
class ComponentAudioSource;
class ComponentMeshRenderer;
class ResourcePrefab;
class HUDController;
class PlayerController;
//class PlayerDeath;
class AIMovement;

enum class Phase {
	PHASE1,
	PHASE2,
	PHASE3
};

class AIDuke : public Script
{
	GENERATE_BODY(AIDuke);

public:

	void Start() override;
	void Update() override;
	void OnAnimationFinished() override;
	void OnAnimationSecondaryFinished() override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;

public:
	UID dukeUID = 0;
	UID playerUID = 0;

	GameObject* duke = nullptr;
	GameObject* player = nullptr;

	Duke dukeCharacter = Duke();

	Phase phase = Phase::PHASE1;

	float shieldCooldown = 0.f;
	float shieldActiveTime = 5.f;

	float bulletHellCooldown = 0.f;
	float bulletHellActiveTime = 5.f;

	float movingTime = 10.f;

	float stunDuration = 3.f;

private:

	ComponentAnimation* animation = nullptr;
	ComponentTransform* ownerTransform = nullptr;
	AIMovement* movementScript = nullptr;

	PlayerController* playerController = nullptr;
	//PlayerDeath* playerDeath = nullptr;

	float currentShieldCooldown = 0.f;
	float currentShieldActiveTime = 0.f;

	float currentBulletHellCooldown = 0.f;
	float currentBulletHellActiveTime = 0.f;

	float currentMovingTime = 0.f;

	float lifeThreshold = 0.70f;

	bool activeFireTiles = false;
	bool activeLasers = false;

	bool phase2Reached = false;

	float stunTimeRemaining = 0.f;
};

