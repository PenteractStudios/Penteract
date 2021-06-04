#pragma once

#include "Scripting/Script.h"

class ComponentTransform;
class ComponentAgent;
class AIMeleeGrunt;

class AIMovement : public Script
{
	GENERATE_BODY(AIMovement);

public:

	void Start() override;
	void Update() override;	

	void Seek(const AIState state, const float3& newPosition, int speed);
	

private:

	float3 velocity = float3(0, 0, 0);	
	ComponentTransform* ownerTransform = nullptr;
	ComponentAgent* agent = nullptr;
};

