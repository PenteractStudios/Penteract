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

    GameObject* go = &GetOwner();

    transform = go->GetComponent<ComponentTransform>();

    AddVehicle(vehicle2);
    AddVehicle(vehicle3);
    AddVehicle(vehicle4);
    AddVehicle(vehicle5);
    AddVehicle(vehicle6);
    AddVehicle(vehicle7);
    AddVehicle(vehicle8);
    AddVehicle(vehicle9);

}

void VehicleLine::Update() {

    if (timeToSpawn <= 0) {
        while (actualVehicle == previousVehicle) {
            actualVehicle = rand() % 9;
        }
        timeToSpawn = timeBetweenSpawns;
        previousVehicle = actualVehicle;
    } 
    else {
        timeToSpawn -= Time::GetDeltaTime();
    }

    UpdateVehicles();
	
}

void VehicleLine::AddVehicle(UID vehicleUID) {

    ResourcePrefab* prefabVehicle = GameplaySystems::GetResource<ResourcePrefab>(vehicleUID);
    GameObject* goVehicle = GameplaySystems::GetGameObject(prefabVehicle->BuildPrefab(&GetOwner()));
    if (prefabVehicle) {
        goVehicle = GameplaySystems::GetGameObject(prefabVehicle->BuildPrefab(&GetOwner()));
        if (goVehicle) {
            ComponentTransform* transform = goVehicle->GetComponent<ComponentTransform>();
            if (transform) {
                vehicles.emplace_back(goVehicle, transform);
            }
        }
    }
}

void VehicleLine::UpdateVehicles()
{
    int vehiclesToClear = 0;
    for (std::pair<GameObject*, ComponentTransform*> vehicle : vehicles) {
        ComponentTransform* transformVehicle = vehicle.first->GetComponent<ComponentTransform>();
        if (!transformVehicle) return;
        float3 position = transformVehicle->GetGlobalPosition();
        float3 direction = transformVehicle->GetGlobalRotation() * float3(1, 0, 0);
        float3 velocity = direction.Normalized() * speed;
        position += velocity * Time::GetDeltaTime();
        transformVehicle->SetGlobalPosition(position);
        if (VehicleOutsideMap(transformVehicle->GetGlobalPosition())){
            transformVehicle = vehicle.second;
        }
    }
    
}

bool VehicleLine::VehicleOutsideMap(const float3 posVehicle)
{
    if (posVehicle.x < -limits.x || posVehicle.x > limits.x) return true;
    if (posVehicle.y < -limits.y || posVehicle.y > limits.y) return true;
    if (posVehicle.z < -limits.z || posVehicle.z > limits.z) return true;
    return false;
}
