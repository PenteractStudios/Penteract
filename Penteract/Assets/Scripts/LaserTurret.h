#pragma once

#include "Scripting/Script.h"

class ComponentAnimation;

class LaserTurret : public Script
{
	GENERATE_BODY(LaserTurret);

	enum class TurretState {
		START = 0,
		SHOOT,
		END
	};

public:

	void Start() override;
	void Update() override;

	void Init();

	void OnAnimationFinished() override;

	UID laserTargetUID = 0;

private:

	// Tiempo de animacion
	// Cooldown --> Tiempo inactivo + factor
	// Factor random --> +/- z tiempo

	ComponentAnimation* animationComp = nullptr;
	TurretState currentState = TurretState::START;
	
	GameObject* laserObject = nullptr;

	const char* states[3] = { "Startup", "Shoot", "End" };

};

