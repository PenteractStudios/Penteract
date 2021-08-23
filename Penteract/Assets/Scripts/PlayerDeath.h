#pragma once

#include "Scripting/Script.h"
#include "AIMeleeGrunt.h"

class PlayerController;
class SceneTransition;

class PlayerDeath : public Script
{
	GENERATE_BODY(PlayerDeath);

public:

	void Start() override;
	void Update() override;
	void OnAnimationFinished() override;
	void OnAnimationSecondaryFinished() override;
	void OnAnimationEvent(StateMachineEnum stateMachineEnum, const char* eventName) override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;

public:

	UID playerUID = 0;
	UID sceneUID = 0;
	UID transitionUID = 0;
	GameObject* player = nullptr;
	PlayerController* playerController = nullptr;
	AIMeleeGrunt* rootParentScript = nullptr;
	bool dead = false;
	float rangedDamageTaken = 1.0f;
	float meleeDamageTaken = 1.0f;
	float barrelDamageTaken = 1.0f;
	float laserBeamTaken = 1.0f;

	float laserHitCooldownTimer = 0.0f;
	float laserHitCooldown = 0.05f;

private:
	GameObject* transitionGO = nullptr;
	SceneTransition* sceneTransition = nullptr;

	bool getLaserHit = false;
};

