#pragma once

#include "Scripting/Script.h"

class ComponentAnimation;
class ComponentAudioSource;

class BossLaserGenerator : public Script {
	GENERATE_BODY(BossLaserGenerator);

	enum class GeneratorState {
		START = 0,
		SHOOT,
		IDLE,
		OFF
	};

public:
	void Start() override;
	void Update() override;

	void Init();
	void StopAudio();

	UID laserTargetUID = 0;
	UID laserWarningUID = 0;
	UID pairGeneratorUID = 0;

	float coolDownOnTimer = 0.0f;
	float coolDownOn = 2.0f;

	float coolDownOffTimer = 0.0f;
	float coolDownOff = 2.0f;

	float chargingDuration = 1.f;

private:
	ComponentAnimation* animationComp = nullptr;
	ComponentAnimation* pairAnimationComp = nullptr;
	ComponentAudioSource* audioComp = nullptr;
	ComponentAudioSource* pairAudioComp = nullptr;
	GeneratorState currentState = GeneratorState::OFF;

	GameObject* laserObject = nullptr;
	ComponentAudioSource* laserAudio = nullptr;
	GameObject* laserWarning = nullptr;
	ComponentAudioSource* laserWarningAudio = nullptr;
	BossLaserGenerator* pairScript = nullptr;

	bool beingUsed = false;

	float chargingTimer = 0.f;

	const std::string states[3] = { "Shooting", "Shooting", "Idle"};
};
