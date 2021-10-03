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
	void SetDronesFormation(DronesFormation newFormation);
	void AddDrone();
	void RemoveDrone();

public:
	UID dronesParentUID = 0;
	UID dronePrefabUID = 0;

	float bulletHellDelay = 0.5f;		// Time that needs to pass until the bullet hell starts to fire
	
private:
	void RecalculateFormations();
	void RepositionDrones();

	std::vector<float3> GenerateLineFormation();
	std::vector<float3> GenerateArrowFormation();
	std::vector<float3> GenerateCircleFormation();

	float GetVerticalOffset() const;

	void StartWave();

private:
	ComponentTransform* transform = nullptr;
	float3 currentPosition = float3(0.0f, 0.0f, 0.0f);
	Quat currentRotation = Quat(0.0f, 0.0f, 0.0f, 0.0f);
	
	GameObject* dronesParent = nullptr;
	std::vector<AttackDroneBehavior*> dronesScripts;

	// Formation
	DronesFormation formation = DronesFormation::LINE;
	std::vector<std::vector<float3>> formationsOffsetPositions;

	// Wave cycle
	WaveCycle cycle = WaveCycle::CENTERED;

	AttackDronesPattern chosenPattern;

	// Bullet Hell shoot start
	bool mustStartBulletHell = false;
	float currentTime = 0.0f;
};