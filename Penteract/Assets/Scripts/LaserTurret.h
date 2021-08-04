#pragma once

#include "Scripting/Script.h"

class ComponentAnimation;

class LaserTurret : public Script
{
	GENERATE_BODY(LaserTurret);

	enum class TurretState {
		START = 0,
		SHOOT,
		END,
		IDLE_START,
		IDLE_END
	};

public:

	void Start() override;
	void Update() override;

	void Init();

	void OnAnimationFinished() override;

	UID laserTargetUID = 0;

	float coolDownOnTimer = 0.0f;
	float coolDownOn = 0.05;

	float coolDownOffTimer = 0.0f;
	float coolDownOff = 0.05;

private:


	ComponentAnimation* animationComp = nullptr;
	TurretState currentState = TurretState::START;
	
	GameObject* laserObject = nullptr;

	const std::string states[5] = { "Startup", "Shoot", "End", "IdleStart", "IdleEnd"};

};

