#pragma once

#include "Scripting/Script.h"

#include <utility>
#include <vector>

class ComponentTransform;
class GameObject;

class VehicleLine : public Script
{
	GENERATE_BODY(VehicleLine);

public:
	
	void Start() override;
	void Update() override;

private:

	void AddVehicle(UID vehicleUID);
	void UpdateVehicles();
	bool VehicleOutsideMap(const float3 posVehicle);

public:

	float speed = 0.f;
	float timeBetweenSpawns = 0.f;

	float3 limits = { 600, 600, 600 };

	UID vehicle1 = 0;
	UID vehicle2 = 0;
	UID vehicle3 = 0;
	UID vehicle4 = 0;
	UID vehicle5 = 0;
	UID vehicle6 = 0;
	UID vehicle7 = 0;
	UID vehicle8 = 0;
	UID vehicle9 = 0;

private:
	std::vector<std::pair<GameObject*, ComponentTransform*>> vehicles;
	
	float timeToSpawn = 0.f;
	int actualVehicle = 0;
	int previousVehicle = 0;

	ComponentTransform* transform = nullptr;
	
};

