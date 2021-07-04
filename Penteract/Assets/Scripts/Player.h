#pragma once

#include "Character.h"
#include "Math/float3.h"

class CameraController;

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
		attackSpeed(attackSpeed_) {
		lifePoints = lifePoints_;
		movementSpeed = movementSpeed_;
		damageHit = damageHit_;
		SetTotalLifePoints(lifePoints);
	}

	// ------- Core Functions ------ //
	void SetAttackSpeed(float attackSpeed_);
	virtual void GetHit(float damage_) override;
	virtual void OnAnimationFinished() = 0;

	void LookAtMouse();
	MovementDirection GetInputMovementDirection() const;
	float3 GetDirection() const;
	virtual void Shoot() {}
	virtual void Update(bool lastInputGamepad = false, bool lockMovement = false);
	virtual void CheckCoolDowns(bool noCooldownMode = false) {}
	int GetMouseDirectionState();
	bool IsActive();
	void IncreaseUltimateCounter();
public:
	float rangedDamageTaken = 1.0f;
	float meleeDamageTaken = 1.0f;
	float attackSpeed = 1.0f;
	float attackCooldownRemaining = 0.f;

	float normalAngularSpeed = 5.0f;
	bool shooting = false;
	bool shootingOnCooldown = false;

	float3 lookAtMousePlanePosition = float3(0, 0, 0);
	ComponentCamera* lookAtMouseCameraComp = nullptr;
	CameraController* cameraController = nullptr;
	ComponentAudioSource* playerAudios[static_cast<int>(AudioPlayer::TOTAL)] = { nullptr };
	float3 facePointDir = float3(0, 0, 0);
	MovementDirection movementInputDirection = MovementDirection::NONE;
	ComponentTransform* playerMainTransform = nullptr;
	int ultimateChargePoints = 0;
	const int ultimateChargePointsTotal = 10;

private:
	void MoveTo();
	virtual bool CanShoot();
	void ResetSwitchStatus();
	MovementDirection GetControllerMovementDirection() const;
	float2 GetControllerOrientationDirection() const;
	void LookAtGamepadDir();
	void LookAtFacePointTarget();
private:
	float currentSwitchDelay = 0.f;
	bool playSwitchParticles = true;
	bool switchInProgress = false;
	float switchDelay = 0.37f;
	bool lastInputGamepad = true;
};
