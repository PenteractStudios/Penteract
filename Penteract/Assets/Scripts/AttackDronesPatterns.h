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
	AttackDronesPattern() {}
	AttackDronesPattern(float droneSeparationHorizontal_, float droneSeparationDepth_, float droneRadiusFormation_, float droneVerticalOffset_, float droneSeparationFromCenter_,
		                float droneShotDelay_, int dronesNumber_, DronesFormation droneFormation_, int waves_, float timeBetweenWaves_, std::vector<WaveCycle> cycles_, int pickChance_,
						bool mustRotateDrones_)
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
		, mustRotateDrones(mustRotateDrones_) {}

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

	int pickChance = 20;

	bool mustRotateDrones = false;
};

extern inline std::vector<AttackDronesPattern> patterns = {
	AttackDronesPattern(),

	AttackDronesPattern(3.0f, 4.0f, 6.0f, 1.0f, 1.0f, 
						0.1f, 5, DronesFormation::ARROW, 5, 0.5f, { WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED }, 30,
						false),

	AttackDronesPattern(3.0f, 4.0f, 6.0f, 1.0f, 1.0f, 
						0.1f, 9, DronesFormation::CIRCLE, 2, 0.5f, { WaveCycle::CENTERED, WaveCycle::CENTERED, WaveCycle::CENTERED }, 30,
						true),

	AttackDronesPattern(3.0f, 4.0f, 6.0f, 1.0f, 1.0f,
						0.3f, 5, DronesFormation::LINE, 3, 0.5f, { WaveCycle::LEFT_TO_RIGHT, WaveCycle::RIGHT_TO_LEFT, WaveCycle::LEFT_TO_RIGHT }, 20,
						false),

};