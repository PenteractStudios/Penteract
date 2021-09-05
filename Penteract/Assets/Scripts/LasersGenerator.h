#pragma once

#include "Scripting/Script.h"

#define CHARGING_DURATION 1.f

class ComponentAnimation;

class LasersGenerator : public Script
{
	GENERATE_BODY(LasersGenerator);

	enum class GeneratorState {
		START = 0,
		SHOOT,
		IDLE
	};

public:

	void Start() override;
	void Update() override;

	void Init();

	UID laserTargetUID = 0;
	UID pairGeneratorUID = 0;

	float coolDownOnTimer = 0.0f;
	float coolDownOn = 2.0f;

	float coolDownOffTimer = 0.0f;
	float coolDownOff = 2.0f;

private:

	ComponentAnimation* animationComp = nullptr;
	ComponentAnimation* pairAnimationComp = nullptr;
	GeneratorState currentState = GeneratorState::IDLE;

	GameObject* laserObject = nullptr;

	float chargingTimer = 0.f;

	const std::string states[3] = { "Shooting", "Shooting", "Idle"};
};

