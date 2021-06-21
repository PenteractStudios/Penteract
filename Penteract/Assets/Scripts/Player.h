#pragma once

#include "Character.h"
#include "Math/float3.h"

enum class MovementDirection {
	NONE = 0,
	UP = 1, 
	UP_LEFT = 2, 
	LEFT = 3, 
	DOWN_LEFT = 4,
	DOWN = 5, 
	DOWN_RIGHT = 6, 
	RIGHT = 7, 
	UP_RIGHT = 8
};

enum class AudioPlayer {
	FIRST_ABILITY, //dash, shield
	SECOND_ABILITY, //EMP Field , Energy Blast
	THIRD_ABILITY, // Ultimate
	SWITCH,
	SHOOT,
	HIT,
	DEATH,
	TOTAL
};

class Player : public Character {
public:
	// ------- Contructors ------- //
	Player() {};

	Player(int lifePoints_, float movementSpeed_, int damageHit_, float attackSpeed_)
		: 
		attackSpeed(attackSpeed_){
		lifePoints = lifePoints_;
		movementSpeed = movementSpeed_;
		damageHit = damageHit_;
		SetTotalLifePoints(lifePoints);
	}

	// ------- Core Functions ------ //
	void SetAttackSpeed(float attackSpeed_);
	void Hit(int damage_) override;

	//debug
	void SetInvincible(bool status);
	void SetOverpower(bool status);
	void SetNoCooldown(bool status);
	int GetOverPowerMode();

	MovementDirection GetInputMovementDirection() const;
	float3 GetDirection(MovementDirection md) const;
	virtual void Shoot() {}

public:
	int rangedDamageTaken = 1;
	int meleeDamageTaken = 1;
	float attackSpeed = 1.0f;
	float attackCooldownRemaining = 0.f;
	bool shooting = false;
	float3 facePointDir = float3(0, 0, 0);

	ComponentAudioSource* audiosPlayer[static_cast<int>(AudioPlayer::TOTAL)] = { nullptr };

private:

	virtual bool CanShoot() { return !shooting; }
};
