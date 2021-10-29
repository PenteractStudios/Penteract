#pragma once

#include <vector>

enum class DronesFormation {
	LINE,
	ARROW,
	CIRCLE,
	COUNT
};

enum class WaveCycle {
	LEFT_TO_RIGHT,
	RIGHT_TO_LEFT,
	CENTERED
};

class AttackDronesPattern
{

public:
	// Attack Drones Default Constructor
	AttackDronesPattern() {}

	// Attack Drones Pattern Constructor (USE THIS TO ADD NEW PATTERNS)
	AttackDronesPattern(float droneSeparationHorizontal_, float droneSeparationDepth_, float droneRadiusFormation_, float droneVerticalOffset_, float droneSeparationFromCenter_,
						float droneShotDelay_, int dronesNumber_, DronesFormation droneFormation_, int waves_, float timeBetweenWaves_,
						std::vector<WaveCycle> cycles_,
						int pickChance_,
						bool rotationDronesActive_, bool rotationDronesClockwise_, float rotationDronesSpeed_,
						float droneProjectileSpeed_)

		: droneSeparationHorizontal(droneSeparationHorizontal_)
		, droneSeparationDepth(droneSeparationDepth_)
		, droneRadiusFormation(droneRadiusFormation_)
		, droneVerticalOffset(droneVerticalOffset_)
		, droneSeparationFromCenter(droneSeparationFromCenter_)
		, droneShotDelay(droneShotDelay_)
		, dronesNumber(dronesNumber_)
		, droneFormation(droneFormation_)
		, waves(waves_)
		, timeBetweenWaves(timeBetweenWaves_)
		, cycles(cycles_)
		, pickChance(pickChance_)
		, rotationDronesActive(rotationDronesActive_)
		, rotationDronesClockwise(rotationDronesClockwise_)
		, rotationDronesSpeed(rotationDronesSpeed_)
		, droneProjectileSpeed(droneProjectileSpeed_) {}

	float droneSeparationHorizontal = 1.0f;
	float droneSeparationDepth = 1.0f;
	float droneRadiusFormation = 2.0f;
	float droneVerticalOffset = 2.0f;
	float droneSeparationFromCenter = 1.0f;
	float droneShotDelay = 0.2f;
	int dronesNumber = 6;
	DronesFormation droneFormation = DronesFormation::LINE;

	int waves = 3;
	float timeBetweenWaves = 1.0f;
	std::vector<WaveCycle> cycles = { WaveCycle::LEFT_TO_RIGHT, WaveCycle::LEFT_TO_RIGHT, WaveCycle::LEFT_TO_RIGHT };

	int pickChance = 30;

	bool rotationDronesActive = false;
	bool rotationDronesClockwise = true;
	float rotationDronesSpeed = 4.0f;

	float droneProjectileSpeed = 1.0f;
};


//How to add a new Pattern:
//	1- Create a new AttackDronesPattern object inside the patterns array
//	2- Set values following the AttackDronesPattern constructor.
//	3- !!!IMPORTANT!!! It's quite important to properly set the pickChance, or else, the pattern may never be picked.
//	4- !!!IMPORTANT!!! The cycles vector MUST have a value for each WAVE, for example, if it has 3 waves, it MUST HAVE an array of 3 cycles.

extern inline std::vector<AttackDronesPattern> patterns = {

	AttackDronesPattern(3.0f, 4.0f, 6.0f, 2.0f, 4.0f,
						0.1f, 7, DronesFormation::ARROW, 7, 0.5f,
						{ WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED},
						15,
						false, false, 0.0f,
						4.0f),

	AttackDronesPattern(3.0f, 4.0f, 6.0f, 2.0f, 2.0f,
						0.0f, 9, DronesFormation::CIRCLE, 12, 0.5f,
						{ WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED },
						35,
						true, true, 1.0f,
						1.0f),

	AttackDronesPattern(2.0f, 4.0f, 6.0f, 2.0f, 3.0f,
						0.3f, 7, DronesFormation::LINE, 6, 0.5f,
						{ WaveCycle::LEFT_TO_RIGHT, WaveCycle::RIGHT_TO_LEFT, WaveCycle::LEFT_TO_RIGHT, WaveCycle::RIGHT_TO_LEFT, WaveCycle::LEFT_TO_RIGHT, WaveCycle::RIGHT_TO_LEFT },
						15,
						false, false, 0.0f,
						2.0f),

	AttackDronesPattern(3.0f, 4.0f, 6.0f, 2.0f, 2.0f,
						0.1f, 9, DronesFormation::CIRCLE, 12, 0.5f,
						{ WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED },
						35,
						true, true, 1.0f,
						1.0f),

};