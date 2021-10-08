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

	float3 InitialPosition = float3(0, 0, 0);
	float3 FinalPosition = float3(0, 0, 0);

	bool movingToInit = true;

	float movementSpeed = 1.0f;
	float maxLaserEscale = 2.f;
	float minLaserEscale = 1.f;


public:

	void Start() override;
	void Update() override;
	void TurnOn();
	void TurnOff();
	bool BeingUsed();

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

