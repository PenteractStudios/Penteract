#pragma once

#include "Character.h"

#include <random>

class ComponentParticleSystem;
class ResourcePrefab;

enum class DukeState {
	BASIC_BEHAVIOUR,
	MELEE_ATTACK,
	SHOOT_SHIELD,
	BULLET_HELL,
	CHARGE,
	CHARGE_ATTACK,
	INVULNERABLE,
	STUNNED,
	PUSHED,
	DEATH
};

/*
AIDuke will Init() the Duke class with all the necessary parameters. AIDuke will just be in charge of managing the behaviour (FSM),
controlling phases and handling Collision and Animation Finished (which can be delegated in turn to Duke class).

Duke class will contain all the boss stats, Animation FSM and agent, as well as the Abilities' methods. Duke class will be in charge
of performing all the movement, abilities, animation and sound triggers when the AIDuke tells it to do so.
*/

class Duke : public Character
{
public:

	enum class DUKE_AUDIOS {
		DEATH,
		TOTAL
	};

	// ------- Contructors ------- //
	Duke() {}

	Duke(float lifePoints_, float movementSpeed_, float damageHit_, float searchRadius_, float attackRange_, float timeToDie_, float pushBackDistance_, float pushBackSpeed_)
		: searchRadius(searchRadius_)
		, attackRange(attackRange_)
		, timeToDie(timeToDie_)
		, pushBackDistance(pushBackDistance_)
		, pushBackSpeed(pushBackSpeed_) {
		lifePoints = lifePoints_;
		movementSpeed = movementSpeed_;
		damageHit = damageHit_;
		SetTotalLifePoints(lifePoints);
	}

	// ------- Core Functions ------ //
	void Init(UID dukeUID, UID playerUID, UID bulletUID, UID barrelUID, UID chargeColliderUID, UID meleeAttackColliderUID, UID chargeAttackColliderUID);
	void ShootAndMove(const float3& playerDirection);
	void MeleeAttack();
	void BulletHell();
	void InitCharge(DukeState nextState);
	void UpdateCharge(bool forceStop = false);
	void CallTroops();
	void Shoot();
	void ThrowBarrels();

	// ------- Animation Functions ------ //
	void OnAnimationFinished();
	void OnAnimationSecondaryFinished();
	void OnAnimationEvent(StateMachineEnum stateMachineEnum, const char* eventName);

public:
	float chargeSpeed = 5.f;
	float chargeMinimumDistance = 10.f;
	float searchRadius = 8.f;
	float attackRange = 2.0f;
	float attackSpeed = 0.5f;
	int attackBurst = 3;
	float timeInterBurst = 1.0f;
	float timeToDie = 5.f;
	float pushBackDistance = 5.f;
	float pushBackSpeed = 5.f;
	float slowedDownSpeed = 3.f;
	float slowedDownTime = 2.f;
	float barrelDamageTaken = 3.f;
	float moveChangeEvery = 2.0f;
	float distanceCorrectEvery = 2.0f;

	DukeState state = DukeState::BASIC_BEHAVIOUR;
	bool criticalMode = false;

	// Effects' states
	bool beingPushed = false;
	bool slowedDown = false;
	bool reducedDamaged = false;

	float3 chargeTarget;

private:
	void InstantiateBarrel();

private:
	GameObject* player = nullptr;
	GameObject* chargeCollider = nullptr;
	GameObject* meleeAttackCollider = nullptr;
	GameObject* chargeAttack = nullptr;
	ComponentTransform* dukeTransform = nullptr;

	bool hasMeleeAttacked = false;

	float3 perpendicular;
	float movementTimer = 0.f;
	float movementChangeThreshold = 2.0f;
	float distanceCorrectionTimer = 0.f;
	float distanceCorrectionThreshold = 2.0f;

	// Charge
	bool trackingChargeTarget = false;

	// Shooting
	float attackTimePool = 0.f;
	ComponentParticleSystem* bullet = nullptr;

	GameObject* meshObj = nullptr;	//Main mesh for Getting MeshRenderer reference and checking frustum presence (if not inside frustum shooting won't happen)

	ResourcePrefab* barrel = nullptr;
	bool instantiateBarrel = false;

	// Animations
	enum DUKE_ANIMATION_STATES {
		CHARGE,
		CHARGE_END,
		CHARGE_START,
		PUNCH,
		DEATH,
		IDLE,
		ENRAGE,
		SHOOT,
		PDA,
		PUSHED,
		SHIELD,
		SHOOT_SHIELD,
		STUN,
		WALK_BACK,
		WALK_FORWARD,
		WALK_NO_AIM,
		WALK_LEFT,
		WALK_RIGHT,
		LENGTH };
	std::string animationStates[static_cast<int>(DUKE_ANIMATION_STATES::LENGTH)] = { "Charge", "ChargeEnd", "ChargeStart",
		"Punch", "Death", "Idle", "Enrage", "Shooting", "PDA", "Pushed", "Shield", "ShootingShield", "Stun",
		"WalkBack", "WalkForward", "WalkForwardNoAim", "WalkLeft", "WalkRight"};

	// Audios
	ComponentAudioSource* dukeAudios[static_cast<int>(DUKE_AUDIOS::TOTAL)] = { nullptr };

	DukeState nextState = DukeState::BASIC_BEHAVIOUR;
	std::random_device rd;
	std::minstd_rand gen;
};
