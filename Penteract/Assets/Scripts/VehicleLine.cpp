#include "VehicleLine.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "Resources/ResourcePrefab.h"

#include <math.h>

EXPOSE_MEMBERS(VehicleLine) {
    MEMBER(MemberType::PREFAB_RESOURCE_UID, vehicle1),
    MEMBER(MemberType::PREFAB_RESOURCE_UID, vehicle2),
    MEMBER(MemberType::PREFAB_RESOURCE_UID, vehicle3),
    MEMBER(MemberType::PREFAB_RESOURCE_UID, vehicle4),
    MEMBER(MemberType::PREFAB_RESOURCE_UID, vehicle5),
    MEMBER(MemberType::PREFAB_RESOURCE_UID, vehicle6),
    MEMBER(MemberType::PREFAB_RESOURCE_UID, vehicle7),
    MEMBER(MemberType::PREFAB_RESOURCE_UID, vehicle8),
    MEMBER(MemberType::PREFAB_RESOURCE_UID, vehicle9),
    MEMBER(MemberType::FLOAT, speed),
    MEMBER(MemberType::FLOAT, timeBetweenSpawns)
};

GENERATE_BODY_IMPL(VehicleLine);

void VehicleLine::Start() {

    vehiclesUID[1] = vehicle1;
    vehiclesUID[2] = vehicle2;
	
}

void VehicleLine::Update() {

    UpdateVehicles();
    if (timeToSpawn <= 0) {
        while (actualVehicle == previousVehicle) {
            actualVehicle = rand() % 9;
        }
        ResourcePrefab* prefabVehicle = GameplaySystems::GetResource<ResourcePrefab>(vehiclesUID[actualVehicle]);
        GameObject* goVehicle = nullptr;
        if (prefabVehicle) goVehicle = GameplaySystems::GetGameObject(prefabVehicle->BuildPrefab(&GetOwner()));
        if(goVehicle) vehicles.push_back(goVehicle);
        timeToSpawn = timeBetweenSpawns;
        previousVehicle = actualVehicle;
    } 
    else {
        timeToSpawn -= Time::GetDeltaTime();
    }
	
}

void VehicleLine::UpdateVehicles()
{
    for (GameObject* vehicle : vehicles) {
        ComponentTransform* transformVehicle = vehicle->GetComponent<ComponentTransform>();
        float3 position = transformVehicle->GetGlobalPosition();
        float3 velocity = position.Normalized() * speed;
        position += velocity * Time::GetDeltaTime();
        transformVehicle->SetGlobalPosition(position);
        if (VehicleOutsideMap(transformVehicle->GetGlobalPosition())){
            GameplaySystems::DestroyGameObject(vehicle);
        }
    }
}

bool VehicleLine::VehicleOutsideMap(const float3 posVehicle)
{
    if (posVehicle.x < -300 || posVehicle.x > 300) return true;
    if (posVehicle.y < -300 || posVehicle.y > 300) return true;
    if (posVehicle.z < -300 || posVehicle.z > 300) return true;
    return false;
}
