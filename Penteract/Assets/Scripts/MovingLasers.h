#pragma once

#include "Scripting/Script.h"
class ComponentAnimation;
class ComponentTransform;
class MovingLasers : public Script
{
	GENERATE_BODY(MovingLasers);

	enum class GeneratorState {
		START = 0,
		SHOOT,
		IDLE
	};

public:

	UID laserTargetUID = 0;
	UID laserWarningUID = 0;
	UID pairGeneratorUID = 0;

	float chargingDuration = 1.f;

	float3 initialGeneratorPosition = float3(0, 0, 0);
	float3 finalGeneratorPosition = float3(0, 0, 0);

	bool movingToInit = true;

	float movementSpeed = 1.0f;

	float3 minLaserEscale = float3(1.f, 1.f, 1.f);
	float3 maxLaserEscale = float3(1.f, 2.05f,1.f);

	float3 minLaserPosition = float3(10.f, 4.f, 0.f);
	float3 maxLaserPosition = float3(20.f, 4.f, 0.f);

public:

	void Start() override;
	void Update() override;
	void TurnOn();
	void TurnOff();
	bool BeingUsed();
	void Synchronize(bool movingToInit);

private:

	ComponentAnimation* animationComp = nullptr;
	ComponentAnimation* pairAnimationComp = nullptr;
	GeneratorState currentState = GeneratorState::IDLE;
	ComponentTransform* transform = nullptr;
	ComponentTransform* laserTransform = nullptr;
	GameObject* laserObject = nullptr;
	GameObject* laserWarning = nullptr;
	MovingLasers* pairScript = nullptr;

	bool beingUsed = false;

	float chargingTimer = 0.f;

	const std::string states[3] = { "Shooting", "Shooting", "Idle" };

};

