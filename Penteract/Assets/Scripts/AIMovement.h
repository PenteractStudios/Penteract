#pragma once

#include "Scripting/Script.h"

#include "AIState.h"

class ComponentTransform;
class ComponentAgent;


class AIMovement : public Script {
	GENERATE_BODY(AIMovement);

public:

	void Start() override;
	void Update() override;

	void Seek(AIState state, const float3& newPosition, int speed, bool orientateToDir);
	void Flee(AIState state, const float3& fromPosition, int speed, bool orientateToDir);
	void Stop();
	bool CharacterInSight(const GameObject* character, const float searchRadius);
	bool CharacterInAttackRange(const GameObject* character, const float meleeRange);

public:
	static int maxAcceleration;
private:

	float3 velocity = float3(0, 0, 0);
	ComponentTransform* ownerTransform = nullptr;
	ComponentAgent* agent = nullptr;
};
