#pragma once

#include "Scripting/Script.h"

class ComponentAudioSource;
class ComponentAnimation;

class LaserTurret : public Script
{
	GENERATE_BODY(LaserTurret);

	enum class Audios {
		UP,
		DOWN,
		TOTAL
	};

	enum class TurretState {
		START = 0,
		SHOOT,
		END,
		IDLE_START,
		SHOOTING_END
	};

public:

	void Start() override;
	void Update() override;

	void Init();

	void OnAnimationFinished() override;

	UID laserTargetUID = 0;
	UID laserWarningUID = 0;

	float coolDownOnTimer = 0.0f;
	float coolDownOn = 2.0f;

	float coolDownOffTimer = 0.0f;
	float coolDownOff = 2.0f;

private:


	ComponentAnimation* animationComp = nullptr;
	TurretState currentState = TurretState::IDLE_START;

	GameObject* laserObject = nullptr;
	ComponentAudioSource* laserAudio = nullptr;
	GameObject* laserWarning = nullptr;

	const std::string states[5] = { "Startup", "Shoot", "End", "StartUpIdle", "EndIdle"};

	ComponentAudioSource* audios[static_cast<int>(Audios::TOTAL)] = { nullptr };

};

