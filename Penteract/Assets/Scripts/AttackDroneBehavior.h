#pragma once

#include "Scripting/Script.h"
#include "ProjectileController.h"

class ComponentTransform;
class ComponentParticleSystem;

class AttackDroneBehavior : public Script
{
	GENERATE_BODY(AttackDroneBehavior);

public:

	void Start() override;
	void Update() override;

	void Reposition(float3 newPosition);
	void SetPositionOffset(float3 newOffset);
	void SetMustForceRotation(bool mustForce);
	void Shoot();
	void StartWave(int newWaves, float bulletDelay, float timeBetweenWaves);

public:
	UID dronesControllerUID = 0;
	float droneSpeed = 8.0f;
	float droneRotationSpeed = 9.0f;

	UID projectilePrefabUID = 0;

private:
	GameObject* dronesController = nullptr;
	ComponentTransform* dronesControllerTransform = nullptr;
	ComponentTransform* transform = nullptr;

	ProjectileController shooter;

	bool mustReposition = false;
	float3 currentPosition = float3(0.0f, 0.0f, 0.0f);
	float3 positionOffset = float3(0.0f, 0.0f, 0.0f);

	bool mustForceRotation = false;

	// Shooting
	int remainingWaves = 0;
	float delay = 0.0f;
	float currentTime = 0.0f;
};

