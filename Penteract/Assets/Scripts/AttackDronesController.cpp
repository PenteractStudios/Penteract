#include "AttackDronesController.h"

#include "GameplaySystems.h"

#include "GameObject.h"
#include "Components/ComponentTransform.h"
#include "Resources/ResourcePrefab.h"
#include "AttackDroneBehavior.h"

#define PI 3.14159

EXPOSE_MEMBERS(AttackDronesController) {
    MEMBER(MemberType::GAME_OBJECT_UID, dronesParentUID),
    MEMBER(MemberType::PREFAB_RESOURCE_UID, dronePrefabUID)
};

GENERATE_BODY_IMPL(AttackDronesController);

void AttackDronesController::Start() {
    transform = GetOwner().GetComponent<ComponentTransform>();
    dronesParent = GameplaySystems::GetGameObject(dronesParentUID);
    if (dronesParent) {
        for (GameObject* drone : dronesParent->GetChildren()) {
            AttackDroneBehavior* droneScript = GET_SCRIPT(drone, AttackDroneBehavior);
            if (droneScript) {
                dronesScripts.push_back(droneScript);
            }
        }
    }

    RecalculateFormations();
    SetDronesFormation(formation);
}

void AttackDronesController::Update() {
    float3 newCurrentPosition = transform->GetGlobalPosition();

    if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_I)) {
        SetDronesFormation(DronesFormation::LINE);
    }

    if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_O)) {
        SetDronesFormation(DronesFormation::ARROW);
    }

    if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_P)) {
        SetDronesFormation(DronesFormation::CIRCLE);
    }

    if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_9)) {
        AddDrone();
    }

    if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_0)) {
        RemoveDrone();
    }
}

void AttackDronesController::SetDronesFormation(DronesFormation newFormation) {
    formation = newFormation;
    RepositionDrones();
}

void AttackDronesController::AddDrone() {
    if (dronePrefabUID == 0 || !dronesParent) return;

    ResourcePrefab* dronePrefab = GameplaySystems::GetResource<ResourcePrefab>(dronePrefabUID);
    GameObject* drone = GameplaySystems::GetGameObject(dronePrefab->BuildPrefab(dronesParent));       // Could be nice if BuildPrefab accepted nullptrs as parent so reparenting does not happen if the process failed

    bool failed = false;
    if (drone) {
        AttackDroneBehavior* droneBehavior = GET_SCRIPT(drone, AttackDroneBehavior);
        if (droneBehavior) {
            dronesScripts.push_back(droneBehavior);
        }
        else {
            failed = true;
        }
    }
    else {
        failed = true;
    }

    if (failed) {
        dronesParent->RemoveChild(drone);
        GameplaySystems::DestroyGameObject(drone);
    }
    else {
        RecalculateFormations();
        RepositionDrones();
    }
}

void AttackDronesController::RemoveDrone() {
    if (dronesScripts.size() == 0 || !dronesParent) return;

    std::vector<GameObject*> drones = dronesParent->GetChildren();

    if (drones.size() != dronesScripts.size()) return;
    GameObject* droneToDestroy = drones[drones.size() - 1];
    dronesParent->RemoveChild(droneToDestroy);
    dronesScripts.pop_back();
    GameplaySystems::DestroyGameObject(droneToDestroy);

    RecalculateFormations();
    RepositionDrones();
}

void AttackDronesController::RecalculateFormations() {
    if (dronesScripts.size() == 0) return;

    formationsOffsetPositions.clear();
    formationsOffsetPositions.resize(static_cast<int>(DronesFormation::COUNT));

    formationsOffsetPositions[0] = GenerateLineFormation();
    formationsOffsetPositions[1] = GenerateArrowFormation();
    formationsOffsetPositions[2] = GenerateCircleFormation();
}

void AttackDronesController::RepositionDrones() {
    for (int i = 0; i < dronesScripts.size(); ++i) {
        dronesScripts[i]->SetPositionOffset(formationsOffsetPositions[static_cast<int>(formation)][i]);
    }
}

std::vector<float3> AttackDronesController::GenerateLineFormation() {
    int size = dronesScripts.size();
    std::vector<float3> result(size);

    for (int i = 0; i < size / 2; ++i) {
        float xSeparation = -((size / 2) - i) * droneSeparationHorizontal;
        result[i] = float3(xSeparation, 0, 0);
    }

    for (int i = (size / 2); i < size; ++i) {
        float xSeparation = (i - (size / 2)) * droneSeparationHorizontal;
        result[i] = float3(xSeparation, 0, 0);
    }

    return result;
}

std::vector<float3> AttackDronesController::GenerateArrowFormation() {
    int size = dronesScripts.size();
    std::vector<float3> result(size);

    for (int i = 0; i < size / 2; ++i) {
        float xSeparation = -((size / 2) - i) * droneSeparationHorizontal;
        float zSeparation = -((size / 2) - i) * droneSeparationDepth;

        result[i] = float3(xSeparation, 0, zSeparation);
    }

    for (int i = (size / 2); i < size; ++i) {
        float xSeparation = (i - (size / 2)) * droneSeparationHorizontal;
        float zSeparation = -(i - (size / 2)) * droneSeparationDepth;
        result[i] = float3(xSeparation, 0, zSeparation);
    }

    return result;
}

std::vector<float3> AttackDronesController::GenerateCircleFormation() {
    int size = dronesScripts.size();
    std::vector<float3> result(size);

    for (int i = 0; i < size; ++i) {
        float theta = ((PI * 2) / size);
        float angle = (theta * i);

        result[i] = float3(cos(angle), 0, sin(angle)) * droneRadiusFormation;
    }

    return result;
}
