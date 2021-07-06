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
	SPECIAL_SHOOT,
	TOTAL
};

enum class InputActions {
	MOVEMENT,
	ORIENTATION,
	SWITCH,
	SHOOT,
	ABILITY_1,
	ABILITY_2,
	ABILITY_3
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

	void LookAtMouse();
	MovementDirection GetInputMovementDirection(bool useGamepad) const;
	float3 GetDirection() const;
	virtual void Shoot() {}
	virtual void Update(bool lastInputGamepad = false, bool lockMovement = false, bool lockRotation = false);
	virtual void CheckCoolDowns(bool noCooldownMode = false) {}
	virtual bool CanSwitch() const = 0;

	virtual void OnAnimationFinished() = 0;
	virtual void OnAnimationSecondaryFinished() = 0;
	virtual bool IsInstantOrientation(bool useGamepad) const = 0;

	int GetMouseDirectionState();
	bool IsActive();
	void IncreaseUltimateCounter();
	static bool GetInputBool(InputActions action, bool useGamepad = false);
	float2 GetInputFloat2(InputActions action, bool useGamepad = false) const;
	void UpdateFacePointDir(bool useGamepad);
public:
	float rangedDamageTaken = 1.0f;
	float meleeDamageTaken = 1.0f;
	float attackSpeed = 1.0f;
	float attackCooldownRemaining = 0.f;
	bool shooting = false;
	float orientationSpeed = 5.0f;
	float orientationThreshold = 5.0f;
	int ultimateChargePoints = 0;
	const int ultimateChargePointsTotal = 10;
	bool shootingOnCooldown = false;
	float normalOrientationSpeed = 5.0f;

	float3 lookAtMousePlanePosition = float3(0, 0, 0);
	ComponentCamera* lookAtMouseCameraComp = nullptr;
	CameraController* cameraController = nullptr;
	ComponentAudioSource* playerAudios[static_cast<int>(AudioPlayer::TOTAL)] = { nullptr };
	float3 facePointDir = float3(0, 0, 0);
	MovementDirection movementInputDirection = MovementDirection::NONE;
	ComponentTransform* playerMainTransform = nullptr;

protected:
	void MoveTo();

private:
	virtual bool CanShoot();
	void ResetSwitchStatus();
	MovementDirection GetControllerMovementDirection(bool useGamepad = false) const;
	float2 GetControllerOrientationDirection() const;
	void LookAtGamepadDir();
	void LookAtFacePointTarget(bool useGamepad);
private:
	float currentSwitchDelay = 0.f;
	bool playSwitchParticles = true;
	bool switchInProgress = false;
	float switchDelay = 0.37f;
};
