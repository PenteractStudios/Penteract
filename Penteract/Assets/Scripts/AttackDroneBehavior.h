#pragma once

#include "Scripting/Script.h"
#include "ProjectileController.h"

class ComponentTransform;
class ComponentParticleSystem;
class AttackDronesController;

class AttackDroneBehavior : public Script
{
	GENERATE_BODY(AttackDroneBehavior);

public:

	void Start() override;
	void Update() override;

	void Reposition(float3 newPosition);
	void SetPositionOffset(float3 newOffset);
	void SetMustForceRotation(bool mustForce);
	void SetWaitEndOfWave(bool mustWait);
	void SetIsLastDrone(bool isLast);
	void SetControllerScript(AttackDronesController* controllerScript);
	void Shoot();
	void StartWave(int newWaves, float bulletDelay, float timeBetweenWaves);
	void Deploy(float timeToReach);
	void Dismiss(float timeToReach);

public:
	UID dronesControllerUID = 0;
	UID projectilePrefabUID = 0;

	float droneSpeed = 8.0f;
	float droneSpeedOnRecoil = 16.0f;
	float droneRotationSpeed = 9.0f;

	bool droneMustHover = true;
	float droneHoverAmplitude = 0.1f;

	bool droneMustRecoil = true;
	float droneRecoilDistance = 0.6f;
	float droneRecoilTime = 0.2f;

private:
	void Translate();
	void Rotate();
	float3 GetHoverOffset();
	float3 GetRecoilOffset();

private:
	GameObject* dronesController = nullptr;
	ComponentTransform* dronesControllerTransform = nullptr;
	ComponentTransform* transform = nullptr;
	ComponentTransform* dronesContainerTransform = nullptr;
	AttackDronesController* dronesControllerScript = nullptr;

	ProjectileController shooter;

	bool droneDisabled = false;

	bool isDeploying = false;
	float deployTime = 1.0f;
	float currentDeployTime = 0.0f;

	bool isDismissing = false;
	float dismissTime = 1.0f;
	float currentDismissTime = 0.0f;

	bool mustReposition = false;
	float3 positionOffset = float3(0.0f, 0.0f, 0.0f);
	bool mustForceRotation = false;

	bool mustWaitEndOfWave = false;
	bool availableShot = true;

	bool isLastDrone = false;

	// Shooting
	int remainingWaves = 0;
	float delay = 0.0f;
	float currentTime = 0.0f;

	// Hover effect
	float hoverCurrentTime = 0.0f;

	// Recoil effect
	float recoilCurrentTime = 0.0f;
	bool isRecoiling = false;
};