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
		CIRCLE
	};

	void Start() override;
	void Update() override;

	void SetDronesFormation(DronesFormation newFormation);

public:
	UID dronesListUID = 0;

	float droneSeparation = 1.0f;

private:
	void RecalculateFormations();
	void RepositionDrones();

private:
	ComponentTransform* transform = nullptr;
	float3 currentPosition = float3(0.0f, 0.0f, 0.0f);
	
	std::vector<GameObject*> drones;
	std::vector<AttackDroneBehavior*> dronesScripts;

	// Formation

	DronesFormation formation = DronesFormation::LINE;

	std::vector<float3> lineFormation;

};

