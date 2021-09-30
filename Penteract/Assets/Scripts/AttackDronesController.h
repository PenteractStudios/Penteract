#pragma once

#include "Scripting/Script.h"

class ComponentTransform;
class AttackDroneBehavior;

class AttackDronesController : public Script
{
	GENERATE_BODY(AttackDronesController);

public:

	enum class DronesFormation {
		LINE,
		ARROW,
		CIRCLE,
		COUNT
	};

	enum class WaveCycle {
		LEFT_TO_RIGHT,
		RIGHT_TO_LEFT,
		CENTERED
	};

	void Start() override;
	void Update() override;

	void SetDronesFormation(DronesFormation newFormation);
	void AddDrone();
	void RemoveDrone();

public:
	UID dronesParentUID = 0;
	UID dronePrefabUID = 0;

	float droneSeparationHorizontal = 1.0f;
	float droneSeparationDepth = 1.0f;
	float droneSeparationVertical = 1.0f;
	float droneRadiusFormation = 2.0f;
	float droneVerticalOffset = 2.0f;
	float separationFromCenter = 1.0f;

	int waves = 3;
	float timeBetweenWaves = 1.0f; 
	float shotDelay = 0.2f;
	
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
	
	GameObject* dronesParent = nullptr;
	std::vector<AttackDroneBehavior*> dronesScripts;

	// Formation
	DronesFormation formation = DronesFormation::LINE;
	std::vector<std::vector<float3>> formationsOffsetPositions;

	// Wave cycle
	WaveCycle cycle = WaveCycle::CENTERED;
};

