#pragma once

#include "Scripting/Script.h"

class BossSceneCameraTravel : public Script
{
	GENERATE_BODY(BossSceneCameraTravel);

public:

	void Start() override;
	void Update() override;

public:
	float travellingAcceleration = 2.0f;
	float travellingDeceleration = 2.0f;
	float maxTravellingSpeed = 30.0f;
	float startingTravellingSpeed = 10.0f;
	float decelerationDistance = 5.0f;
	float finishDistanceThreshold = 0.3f;

};

