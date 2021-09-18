#pragma once

#include "Character.h"

#include <random>

class ComponentParticleSystem;

enum class DukeState {
	BASIC_BEHAVIOUR,
	MELEE_ATTACK,
	SHOOT_SHIELD,
	BULLET_HELL,
	CHARGE,
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

	Duke(float lifePoints_, float movementSpeed_, float damageHit_, float damageBullet_,float damageCharge_, float searchRadius_, float attackRange_, float timeToDie_, float pushBackDistance_, float pushBackSpeed_)
		: searchRadius(searchRadius_)
		, attackRange(attackRange_)
		, timeToDie(timeToDie_)
		, pushBackDistance(pushBackDistance_)
		, pushBackSpeed(pushBackSpeed_)
		, damageBullet(damageBullet_)
		, damageCharge(damageCharge_) {
		lifePoints = lifePoints_;
		movementSpeed = movementSpeed_;
		damageHit = damageHit_;
		SetTotalLifePoints(lifePoints);
	}

	// ------- Core Functions ------ //
	void Init(UID dukeUID, UID playerUID, UID bulletUID);
	void ShootAndMove(const float3& playerDirection);
	void MeleeAttack();
	void ShieldShoot();
	void BulletHell();
	void Charge(DukeState nextState);
	void CallTroops();
	void Shoot();

public:
	float damageBullet = 1.f;
	float damageCharge = 1.f;
	float chargeSpeed = 5.f;
	float searchRadius = 8.f;
	float attackRange = 2.0f;
	float attackSpeed = 0.5f;
	int attackBurst = 3;
	float timeInterBurst = 1.0f;
	float timeToDie = 5.f;
	float pushBackDistance = 5.f;
	float pushBackSpeed = 5.f;
	float barrelDamageTaken = 3.f;
	float moveChangeEvery = 2.0f;

	DukeState state = DukeState::BASIC_BEHAVIOUR;
	bool criticalMode = false;

	bool isShielding = false;

	float3 chargeTarget;

private:
	GameObject* player = nullptr;
	ComponentTransform* dukeTransform = nullptr;

	float3 perpendicular;
	float movementTimer = 0.f;
	float movementChangeThreshold = 2.0f;

	// Shooting
	float attackTimePool = 0.f;
	ComponentParticleSystem* bullet = nullptr;

	GameObject* meshObj = nullptr;	//Main mesh for Getting MeshRenderer reference and checking frustum presence (if not inside frustum shooting won't happen)

	//Audios
	ComponentAudioSource* dukeAudios[static_cast<int>(DUKE_AUDIOS::TOTAL)] = { nullptr };

	std::random_device rd;
	std::minstd_rand gen;
};
