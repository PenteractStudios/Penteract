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
    
}

void AttackDronesController::SetDronesFormation(DronesFormation newFormation) {
    for (int i = 0; i < dronesScripts.size(); ++i) {
        dronesScripts[i]->SetPositionOffset(lineFormation[i]);
    }

    /*for (AttackDroneBehavior* droneScript : dronesScripts) {
        droneScript->SetPositionOffset();
    }*/
}

void AttackDronesController::RecalculateFormations() {
    int size = dronesScripts.size();
    if (size == 0) return;

    lineFormation.resize(size);


    // Line formation
    for (int i = 0; i < size / 2; ++i) {
        float xSeparation = -((size / 2) - i) * droneSeparation;
        lineFormation[i] = float3(xSeparation, 0, 0);
    }

    //lineFormation[(size / 2)] = float3(0.0f, 0.0f, 0.0f);

    for (int i = (size / 2); i < size; ++i) {
        float xSeparation = (i - (size / 2)) * droneSeparation;
        lineFormation[i] = float3(xSeparation, 0, 0);
    }

}

void AttackDronesController::RepositionDrones() {
    for (AttackDroneBehavior* droneScript : dronesScripts) {
        droneScript->Reposition(currentPosition);
    }
}
