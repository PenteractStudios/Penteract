#pragma once

#include "Character.h"

#include <random>
#include <vector>

class ComponentParticleSystem;
class ResourcePrefab;
class ResourceMaterial;
class AttackDronesController;
class BarrelSpawner;
class DukeShield;

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

	Duke(float lifePoints_, float movementSpeed_, float damageHit_, float searchRadius_, float attackRange_, float timeToDie_, float pushBackDistance_, float pushBackTime_)
		: searchRadius(searchRadius_)
		, attackRange(attackRange_)
		, timeToDie(timeToDie_)
		, pushBackDistance(pushBackDistance_)
		, pushBackTime(pushBackTime_) {
		lifePoints = lifePoints_;
		movementSpeed = movementSpeed_;
		damageHit = damageHit_;
		SetTotalLifePoints(lifePoints);
	}

	// ------- Core Functions ------ //
	void Init(UID dukeUID, UID playerUID, UID bulletUID, UID barrelUID, UID chargeColliderUID, UID meleeAttackColliderUID, UID barrelSpawnerUID, UID chargeAttackColliderUID, UID phase2ShieldUID, std::vector<UID> encounterUIDs, AttackDronesController* dronesController, UID punchSlashUID, UID chargeDustUID, UID areaChargeUID, UID chargeTelegraphAreaUID, UID chargePunchVFXUID, UID dustStepLeftUID, UID dustStepRightUID, UID bodyArmorUID);
	void ShootAndMove(const float3& playerDirection);
	void MeleeAttack();
	void BulletHell();
	void DisableBulletHell();
	bool BulletHellActive() const;
	bool BulletHellFinished() const;
	bool IsBulletHellCircular() const;
	bool PlayerIsInChargeRangeDistance() const;
	void InitCharge(DukeState nextState);
	void UpdateCharge(bool forceStop = false);
	void UpdateChargeAttack();
	void CallTroops();
	void Move(const float3& playerDirection);
	void Shoot();
	void ThrowBarrels();
	void StartUsingShield();
	void BePushed();
	void BecomeStunned();
	void TeleportDuke(bool toMapCenter);

	// ------- Animation Functions ------ //
	void OnAnimationFinished();
	void OnAnimationSecondaryFinished();
	void OnAnimationEvent(StateMachineEnum stateMachineEnum, const char* eventName);
	void StopShooting();
	void StartPhase2Shield();

	// ---- Auxiliary Functions ---- //
	void ActivateDissolve(UID dissolveMaterialID);
	void SetCriticalMode(bool activate);

	// ------ Getters/Setters ------ //
	ComponentMeshRenderer* GetDukeMeshRenderer() const;

private:
	int GetWalkAnimation();

public:
	float chargeSpeed = 5.f;
	float chargeMinimumDistance = 10.f;
	float searchRadius = 8.f;
	float attackRange = 2.0f;
	float attackSpeed = 0.5f;
	int attackBurst = 3;
	float timeInterBurst = 1.0f;
	float timeToDie = 5.f;
	float pushBackDistance = 7.f;
	float pushBackTime = 1.f;
	float slowedDownSpeed = 3.f;
	float slowedDownTime = 2.f;
	float barrelDamageTaken = 3.f;
	float moveChangeEvery = 2.0f;
	float distanceCorrectEvery = 2.0f;

	float chargeSkidMaxSpeed = 5.f;
	float chargeSkidMinSpeed = 2.f;
	float chargeSkidDuration = 1.0f;


	DukeShield* phase2Shield = nullptr;

	DukeState state = DukeState::BASIC_BEHAVIOUR;
	bool criticalMode = false;
	bool mustAddAgent = false;

	// Effects' states
	bool beingPushed = false;
	bool slowedDown = false;
	bool reducedDamaged = false;
	bool startSpawnBarrel = false;
	bool isDead = false;
	bool isShooting = false;
	bool isInArena = true;

	float3 chargeTarget;

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
		LENGTH
	};
	std::string animationStates[static_cast<int>(DUKE_ANIMATION_STATES::LENGTH)] = { "Charge", "ChargeEnd", "ChargeStart",
		"Punch", "Death", "Idle", "Enrage", "Shooting", "PDA", "Pushed", "Shield", "ShootingShield", "Stun",
		"WalkBack", "WalkForward", "WalkForwardNoAim", "WalkLeft", "WalkRight" };

	UID winSceneUID = 0; // TODO: Delete

private:
	void InstantiateBarrel();

private:
	GameObject* player = nullptr;
	ComponentTransform* dukeTransform = nullptr;

	bool hasMeleeAttacked = false;

	BarrelSpawner* barrelSpawneScript = nullptr;

	// Movement
	float3 perpendicular;
	float movementTimer = 0.f;
	float movementChangeThreshold = 2.0f;
	float distanceCorrectionTimer = 0.f;
	float distanceCorrectionThreshold = 2.0f;
	bool navigationHit = false;
	float3 navigationHitPos = float3(0,0,0);
	
	// Melee Attack
	GameObject* meleeAttackCollider = nullptr;
	ComponentParticleSystem* punchSlash = nullptr;
	bool firstTimePunchParticlesActive = true;

	// Charge
	GameObject* chargeCollider = nullptr;
	GameObject* chargeAttack = nullptr;
	bool trackingChargeTarget = false;
	ComponentBillboard* chargeTelegraphArea = nullptr;
	GameObject* chargeTelegraphAreaGO = nullptr;
	GameObject* areaChargeGO = nullptr;
	ResourceMaterial* areaCharge = nullptr;
	ComponentParticleSystem* chargeDust = nullptr;
	ComponentParticleSystem* chargePunchVFX = nullptr;
	float2 chargeDustOriginalParticlesPerSecond = float2(0.f, 0.f);
	float areaChargeSpeedMultiplier = 4;
	float dukeScale = 0.f;
	float chargeTelegraphAreaPosOffset = 0.f;

	//Shield
	ComponentParticleSystem* phase2ShieldParticles = nullptr;

	// Shooting
	float attackTimePool = 0.f;
	ComponentParticleSystem* bullet = nullptr;
	float isShootingTimer = 0.f;

	//Enrage
	GameObject* bodyArmor = nullptr;

	//Steps
	ComponentParticleSystem* dustLeftStep = nullptr;
	ComponentParticleSystem* dustRightStep = nullptr;

	GameObject* meshObj = nullptr;	//Main mesh for Getting MeshRenderer reference and checking frustum presence (if not inside frustum shooting won't happen)

	ResourcePrefab* barrel = nullptr;
	bool instantiateBarrel = false;

	// AttackDrones
	AttackDronesController* attackDronesController = nullptr;

	// Audios
	ComponentAudioSource* dukeAudios[static_cast<int>(DUKE_AUDIOS::TOTAL)] = { nullptr };

	DukeState nextState = DukeState::BASIC_BEHAVIOUR;

	/* Boss encounters */
	std::vector<GameObject*> encounters;
	unsigned currentEncounter = 0;

	float3 phase2CenterPosition = float3(13.0f, 0.799f, 0.0f);

	float3 chargeDir = float3(0, 0, 0);
	float chargeSkidTimer = 0.0f;
};
