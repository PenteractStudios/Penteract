#pragma once

#include "Scripting/Script.h"
class ComponentAnimation;
class ComponentTransform;
class ComponentParticleSystem;
class ComponentBoxCollider;
class ComponentAudioSource;

class MovingLasers : public Script
{
	GENERATE_BODY(MovingLasers);

	enum class GeneratorState {
		START = 0,
		SHOOT,
		IDLE,
		DISABLE
	};

public:

	UID laserTargetUID = 0;
	UID laserWarningUID = 0;
	UID pairGeneratorUID = 0;

	float chargingDuration = 1.f;

	float3 initialGeneratorPosition = float3(0, 0, 0);
	float3 finalGeneratorPosition = float3(0, 0, 0);

	bool movingToInit = true;

	float movementSpeed = 10.0f;

	float3 minLaserEscale = float3(1.f, 1.f, 1.f);
	float3 maxLaserEscale = float3(1.f, 2.05f,1.f);

	float3 minLaserColliderSize = float3(0.250f, 20.f, 0.250f);
	float3 maxLaserColliderSize = float3(0.250f, 41.f, 0.250f);

	float3 minLaserPosition = float3(10.f, 4.f, 0.f);
	float3 maxLaserPosition = float3(20.f, 4.f, 0.f);

	float2 LaserWarningStartScale = float2(400.f, 280.f);

public:

	void Start() override;
	void Update() override;
	void TurnOn();
	void TurnOff();
	bool BeingUsed();
	void Synchronize(bool movingToInit);

private:

	bool Move(); // returns true if position = maxPosition || position = minPosition

private:

	ComponentAnimation* animationComp = nullptr;
	ComponentAnimation* pairAnimationComp = nullptr;
	ComponentAudioSource* audioComp = nullptr;

	GeneratorState currentState = GeneratorState::IDLE;
	ComponentTransform* transform = nullptr;

	ComponentTransform* laserTransform = nullptr;
	GameObject* laserObject = nullptr;
	ComponentAudioSource* laserObjectSFX = nullptr;
	ComponentBoxCollider* laserCollider = nullptr;
	GameObject* laserWarning = nullptr;
	MovingLasers* pairScript = nullptr;
	ComponentParticleSystem* laserWarningVFX = nullptr;
	ComponentAudioSource* laserWarningSFX = nullptr;
	bool beingUsed = false;

	float chargingTimer = 0.f;

	const std::string states[3] = { "Shooting", "Shooting", "Idle" };

};

