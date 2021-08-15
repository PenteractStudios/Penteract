#pragma once

#include "Character.h"

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

class Duke : public Character
{
public:
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
	void Init(UID dukeUID, UID playerUID);
	void ShootAndMove(const float3& newPosition);
	void MeleeAttack();
	void ShieldShoot();
	void BulletHell();
	void Charge();
	void CallTroops();

public:
	float damageBullet = 1.f;
	float damageCharge = 1.f;
	float searchRadius = 10.f;
	float attackRange = 2.0f;
	float timeToDie = 5.f;
	float pushBackDistance = 5.f;
	float pushBackSpeed = 5.f;
	float barrelDamageTaken = 3.f;

	DukeState state = DukeState::BASIC_BEHAVIOUR;

private:
	GameObject* player = nullptr;
};
