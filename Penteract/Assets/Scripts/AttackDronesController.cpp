#include "AttackDronesController.h"

#include "GameplaySystems.h"

#include "GameObject.h"
#include "Components/ComponentTransform.h"
#include "AttackDroneBehavior.h"

EXPOSE_MEMBERS(AttackDronesController) {
    MEMBER(MemberType::GAME_OBJECT_UID, dronesListUID)
};

GENERATE_BODY_IMPL(AttackDronesController);

void AttackDronesController::Start() {
    transform = GetOwner().GetComponent<ComponentTransform>();
    GameObject* dronesList = GameplaySystems::GetGameObject(dronesListUID);
    if (dronesList) {
        drones = dronesList->GetChildren();
        for (GameObject* drone : drones) {
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
}

void AttackDronesController::SetDronesFormation(DronesFormation newFormation) {
    formation = newFormation;

    for (int i = 0; i < dronesScripts.size(); ++i) {
        dronesScripts[i]->SetPositionOffset(formationsOffsetPositions[static_cast<int>(formation)][i]);
    }
}

void AttackDronesController::RecalculateFormations() {
    if (dronesScripts.size() == 0) return;

    formationsOffsetPositions.resize(static_cast<int>(DronesFormation::COUNT));

    formationsOffsetPositions[0] = GenerateLineFormation();
    formationsOffsetPositions[1] = GenerateArrowFormation();
    formationsOffsetPositions[2] = GenerateCircleFormation();
}

void AttackDronesController::RepositionDrones() {
    for (AttackDroneBehavior* droneScript : dronesScripts) {
        droneScript->Reposition(currentPosition);
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

std::vector<float3> AttackDronesController::GenerateArrowFormation()
{
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

std::vector<float3> AttackDronesController::GenerateCircleFormation()
{
    return std::vector<float3>();
}
