#pragma once

#include "Scripting/Script.h"
#include "AttackDronesPatterns.h"

#include "Math/Quat.h"

class ComponentTransform;
class AttackDroneBehavior;

class AttackDronesController : public Script
{
	GENERATE_BODY(AttackDronesController);

public:

	void Start() override;
	void Update() override;

	void StartBulletHell();
	bool BulletHellActive() const;
	bool BulletHellFinished() const;
	void EndOfWave();

	void SetDronesFormation(DronesFormation newFormation);
	void AddDrone();
	void RemoveDrone();
	void DeployDrones();
	void DismissDrones();

public:
	UID dronesParentUID = 0;
	UID dronePrefabUID = 0;

	float dronesRepositionDelay = 1.5f;			// Time that needs to position drones
	float dronesStartShootingDelay = 0.25f;		// Time to start shooting
	float dronesDismissDelay = 0.25f;			// Time until drones are dismissed
	
private:
	void RecalculateFormations();
	void RepositionDrones();
	void CheckDronesWaitEndOfWave();

	std::vector<float3> GenerateLineFormation();
	std::vector<float3> GenerateArrowFormation();
	std::vector<float3> GenerateCircleFormation();

	float GetVerticalOffset() const;

	void StartWave();
	void SetDronesWaitEndOfWave(bool value);
	void SetDronesIsLastDrone(bool value);

	bool MustWaitEndOfWave() const;
	bool HadToWaitEndOfWave() const;

private:
	ComponentTransform* transform = nullptr;
	float3 currentPosition = float3(0.0f, 0.0f, 0.0f);
	Quat currentRotation = Quat(0.0f, 0.0f, 0.0f, 0.0f);
	
	GameObject* dronesParent = nullptr;
	std::vector<AttackDroneBehavior*> dronesScripts;

	// Formation
	DronesFormation formation = DronesFormation::LINE;
	std::vector<float3> formationsOffsetPositions;

	// Wave cycle
	WaveCycle cycle = WaveCycle::CENTERED;

	AttackDronesPattern chosenPattern;

	// Bullet Hell shoot start
	bool mustStartBulletHell = false;
	bool mustStopBulletHell = false;
	float currentTime = 0.0f;

	int waves = 0;
	bool bulletHellFinished = true;

	float rotationOffset = 0.0f;
};