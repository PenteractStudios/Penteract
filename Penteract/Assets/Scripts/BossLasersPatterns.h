#pragma once

#include "Scripting/Script.h"
#include <vector>
#include <random>

class LasersGenerator;

class BossLasersPatterns : public Script {
	GENERATE_BODY(BossLasersPatterns);

public:
	void Start() override;
	void Update() override;

	float laserActiveDuration = 2.0f;

	float intervalDuration = 2.0f;

	UID generatorA1UID = 0;
	// UID generatorA2UID = 0;

	UID generatorB1UID = 0;
	// UID generatorB2UID = 0;

	UID generatorC1UID = 0;
	// UID generatorC2UID = 0;

	UID generatorD1UID = 0;
	// UID generatorD2UID = 0;

private:
	float permutationDuration = 0.f;
	float permutationTimer = 0.0f;

	std::vector<LasersGenerator*> generators;

	std::random_device rd;
	std::minstd_rand gen;
};
