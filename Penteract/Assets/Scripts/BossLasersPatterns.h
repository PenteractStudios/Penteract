#pragma once

#include "Scripting/Script.h"
#include <vector>
#include <random>

class BossLaserGenerator;

class BossLasersPatterns : public Script {
	GENERATE_BODY(BossLasersPatterns);

public:
	void Start() override;
	void Update() override;

	void StopAudio();

	float laserActiveDuration = 2.0f;

	float intervalDuration = 2.0f;

	UID generatorA1UID = 0;
	UID generatorB1UID = 0;
	UID generatorC1UID = 0;
	UID generatorD1UID = 0;
	
private:
	float permutationDuration = 0.f;
	float permutationTimer = 0.0f;

	std::vector<BossLaserGenerator*> generators;

	std::random_device rd;
	std::minstd_rand gen;
};
