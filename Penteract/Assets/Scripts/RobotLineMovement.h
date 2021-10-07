#pragma once

#include "Scripting/Script.h"

class ComponentTransform;

class RobotLineMovement : public Script
{
	GENERATE_BODY(RobotLineMovement);

public:

	void Start() override;
	void Update() override;

	void Initialize(const float3& startPos, const float3& dstPos, float _totalTime);

	void Restart();
	void Stop();

	bool NeedsToBeDestroyed() const;

private:

	float3 initialPos = { 0,0,0 };
	float3 finalPos = { 0,0,0 };

	float currentTime = 0.f;
	float totalTime = 0.f;

	ComponentTransform* robotTransform = nullptr;

	bool stopped = false;

	bool destroy = false;

};

