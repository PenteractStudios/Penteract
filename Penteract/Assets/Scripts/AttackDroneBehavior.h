#pragma once

#include "Scripting/Script.h"

class ComponentTransform;

class AttackDroneBehavior : public Script
{
	GENERATE_BODY(AttackDroneBehavior);

public:

	void Start() override;
	void Update() override;

	void Reposition(float3 newPosition);
	void SetPositionOffset(float3 newOffset);

public:
	UID dronesControllerUID = 0;
	float droneSpeed = 8.0f;

private:
	GameObject* dronesController = nullptr;
	ComponentTransform* dronesControllerTransform = nullptr;
	ComponentTransform* transform = nullptr;

	float currentTime = 0.0f;
	bool mustReposition = false;
	float3 currentPosition = float3(0.0f, 0.0f, 0.0f);
	float3 positionOffset = float3(0.0f, 0.0f, 0.0f);

};

