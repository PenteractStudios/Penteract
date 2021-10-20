#pragma once

#include "Scripting/Script.h"
#include "Duke.h"

class GameObject;
class ComponentTransform;
class ComponentAgent;
class ComponentAudioSource;
class ComponentMeshRenderer;
class ResourcePrefab;
class HUDController;
class PlayerController;
class DukeShield;
class Player;
class HUDManager;
//class PlayerDeath;
class AIMovement;
class FloorIsLava;

enum class Phase {
	PHASE1,
	PHASE2,
	PHASE3
};

/*
AIDuke will Init() the Duke class with all the necessary parameters. AIDuke will just be in charge of managing the behaviour (FSM),
controlling phases and handling Collision and Animation Finished (which can be delegated in turn to Duke class).

Duke class will contain all the boss stats, Animation FSM and agent, as well as the Abilities' methods. Duke class will be in charge
of performing all the movement, abilities, animation and sound triggers when the AIDuke tells it to do so.
*/

class AIDuke : public Script
{
	GENERATE_BODY(AIDuke);

public:

	void Start() override;
	void Update() override;
	void OnAnimationFinished() override;
	void OnAnimationSecondaryFinished() override;
	void OnAnimationEvent(StateMachineEnum stateMachineEnum, const char* eventName) override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;
	void SetReady(bool value);
	void EnableBlastPushBack();
	void DisableBlastPushBack();
	bool IsBeingPushed() const;
	float GetDukeMaxHealth() const;

	void ActivateDissolve();

private:
	void CalculatePushBackRealDistance();
	void UpdatePushBackPosition();
	void ParticleHit(GameObject& collidedWith, void* particle, Player& player_);
	bool CanBeHurtDuringCriticalMode() const;
	bool IsInvulnerable()const;
	void OnShieldInterrupted();
	void PerformBulletHell();

public:
	UID dukeUID = 0;
	UID playerUID = 0;
	UID shieldObjUID = 0;
	UID bulletUID = 0;
	UID barrelUID = 0;
	UID chargeColliderUID = 0;
	UID phase2ShieldUID = 0;
	UID firstEncounterUID = 0;
	UID secondEncounterUID = 0;
	UID thirdEncounterUID = 0;
	UID fourthEncounterUID = 0;
	UID meleeAttackColliderUID = 0;
	UID barrelSpawnerUID = 0;
	UID chargeAttackUID = 0;
	UID lasersUID = 0;
	UID videoParentCanvasUID = 0;
	UID videoCanvasUID = 0;
	UID hudManagerUID = 0;
	UID fireTilesUID = 0;
	// Only for level2
	UID triggerBosslvl2EndUID = 0;
	UID dissolveMaterialGOUID = 0;

	GameObject* duke = nullptr;
	GameObject* player = nullptr;

	Duke dukeCharacter = Duke();
	DukeShield* dukeShield = nullptr;

	Phase phase = Phase::PHASE1;

	float shieldCooldown = 0.f;
	float shieldActiveTime = 5.f;

	float bulletHellCooldown = 0.f;

	float abilityChangeCooldown = 8.f;

	float stunDuration = 3.f;

	float troopsCounter = 5.f;

	float throwBarrelTimer = 5.f;

	float orientationSpeed = 1.0f;
	float orientationThreshold = 0.1f;

	float timerBetweenAbilities = 1.5f;

	bool islevel2 = false;

	UID winSceneUID = 0;

private:
	ComponentTransform* ownerTransform = nullptr;
	AIMovement* movementScript = nullptr;

	PlayerController* playerController = nullptr;
	FloorIsLava* fireTilesScript = nullptr;

	HUDManager* hudManager = nullptr;
	GameObject* lasers = nullptr;

	bool isReady = true;

	float currentShieldCooldown = 0.f;
	float currentShieldActiveTime = 0.f;

	float currentBulletHellCooldown = 0.f;
	bool bulletHellIsActive = false;

	float currentAbilityChangeCooldown = 0.f;

	float currentMovingTime = 0.f;

	float currentBarrelTimer = 0.f;

	float lifeThreshold = 0.70f;
	float lasersThreshold = 0.6f;

	bool activeFireTiles = false;

	bool phase2Reached = false;

	float stunTimeRemaining = 0.f;

	float timeSinceLastCharge = 0.f;

	// Onimaru blast effect
	float currentPushBackDistance = 0.f;
	float currentSlowedDownTime = 0.f;
	float pushBackRealDistance = 0.f;

	// Only for level2
	GameObject* triggerBosslvl2End = nullptr;
	UID dissolveMaterialID = 0;

	float currentTimeBetweenAbilities = 0.f;
	bool mustWaitForTimerBetweenAbilities = true;

};

