#pragma once

#include "Scripting/Script.h"

class AIMovement;
class GameObject;
class ComponentTransform;
class ComponentAgent;
class ComponentAnimation;
class ComponentMeshRenderer;
class ComponentParticleSystem;

class RatRobot : public Script {
	GENERATE_BODY(RatRobot);

public:
	enum class RatRobotState {
		IDLE1,
		IDLE2,
		IDLE3,
		RUN
	};

	void Start() override;
	void Update() override;
	void OnAnimationFinished() override;

public:
	UID playerId = 0;
	float fleeRange = 10.0f;
	float steppedOnRange = 1.0f;
	float rotationSmoothness = 0.2f;
	float timeToRespawn = 10.0f;
private:
	void ChangeState(RatRobotState newState);
	void OnRespawn();
	void SteppedOn();
private:
	GameObject* player = nullptr;
	RatRobotState state = RatRobotState::IDLE1;

	ComponentTransform* playerTransform = nullptr;

	ComponentTransform* transform = nullptr;		// Reference to owner transform component
	ComponentAgent* agent = nullptr;				// Reference to Agent component, for navigation
	ComponentAnimation* animation = nullptr;		// Refernece to animation component
	float respawnTimer = 0.0f;
	ComponentMeshRenderer* meshRenderer = nullptr;
	ComponentParticleSystem* explosionParticleSystem = nullptr;
};

