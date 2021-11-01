#pragma once

#include "Scripting/Script.h"
#include <vector>
#include <random>

class LasersGenerator;

class SecurityLasersPatterns : public Script
{
	GENERATE_BODY(SecurityLasersPatterns);

public:

	void Start() override;
	void Update() override;

	void StopAudio();

	float laserActiveDuration = 2.0f;

	float intervalDuration = 2.0f;

	UID generator1UID = 0;
	UID generator2UID = 0;
	UID generator3UID = 0;
	UID generator4UID = 0;

private:
	float permutationDuration = 0.f;
	float permutationTimer = 0.0f;

	std::vector<LasersGenerator*> generators;

	std::random_device rd;
	std::minstd_rand gen;
};

