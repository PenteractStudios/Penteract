#pragma once

#include "Scripting/Script.h"

class ComponentAnimation;

static const char* states[] = { "Start", "Shoot", "End" };

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

	ComponentAnimation* animationComp = nullptr;
	TurretState currentState = TurretState::START;
	
	GameObject* laserObject = nullptr;

};

