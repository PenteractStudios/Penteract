#include "AttackDronesController.h"

#include "GameplaySystems.h"

#include "GameObject.h"
#include "Components/ComponentTransform.h"
#include "Resources/ResourcePrefab.h"
#include "AttackDroneBehavior.h"
#include "AttackDronesPatterns.h"
#include "RandomNumberGenerator.h"

#define PI 3.14159

EXPOSE_MEMBERS(AttackDronesController) {
    MEMBER(MemberType::GAME_OBJECT_UID, dronesParentUID),
    MEMBER(MemberType::PREFAB_RESOURCE_UID, dronePrefabUID),
    MEMBER(MemberType::FLOAT, bulletHellDelay)
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
    if (!currentPosition.Equals(transform->GetGlobalPosition()) || !currentRotation.Equals(transform->GetGlobalRotation())) {
        currentPosition = transform->GetGlobalPosition();

        RecalculateFormations();
        RepositionDrones();
    }

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

    if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_B)) {
        StartBulletHell();
    }

    if (mustStartBulletHell) {
        if (currentTime > bulletHellDelay) {
            StartWave();
            mustStartBulletHell = false;
        }
        else {
            currentTime += Time::GetDeltaTime();
        }
    }
}

void AttackDronesController::StartBulletHell() {
    if (patterns.empty()) return;

    int chance = RandomNumberGenerator::GenerateInteger(1, 100);
    Debug::Log(std::to_string(chance).c_str());
    int accumulatedChance = 0;
    for (AttackDronesPattern pattern : patterns) {
        accumulatedChance += pattern.pickChance;
        if (chance <= accumulatedChance) {
            chosenPattern = pattern;
            break;
        }
    }

    if (chance > accumulatedChance) return;     // hasn't found a pattern

    int currentNumberOfDrones = dronesScripts.size();
    int newNumberOfDrones = chosenPattern.dronesNumber;

    if (currentNumberOfDrones > newNumberOfDrones) {        // Must delete drones
        for (int i = 0; i < currentNumberOfDrones - newNumberOfDrones; ++i) {
            RemoveDrone();
        }
    }
    else if (currentNumberOfDrones < newNumberOfDrones) {
        for (int i = 0; i < newNumberOfDrones - currentNumberOfDrones; ++i) {
            AddDrone();
        }
    }

    RecalculateFormations();
    SetDronesFormation(chosenPattern.droneFormation);
    RepositionDrones();
    mustStartBulletHell = true;
    currentTime = 0.0f;
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
        dronesScripts[i]->SetMustForceRotation(cycle == WaveCycle::CENTERED);
    }
}

std::vector<float3> AttackDronesController::GenerateLineFormation() {
    int size = dronesScripts.size();
    std::vector<float3> result(size);

    float shiftOffset = size % 2 == 0 ? chosenPattern.droneSeparationHorizontal / 2 : 0;

    for (int i = 0; i < size / 2; ++i) {
        float xSeparation = -((size / 2) - i) * chosenPattern.droneSeparationHorizontal;
        
        result[i] = float3x3::RotateY(transform->GetGlobalRotation().ToEulerXZY().z) * float3(xSeparation + shiftOffset, 0.0f, chosenPattern.droneSeparationFromCenter);       // The value of Z is the correct angle since ToEulerXYZ decomposes it into 180,Y,180.
        result[i] += float3(0.0f, GetVerticalOffset(), 0.0f);
    }

    for (int i = (size / 2); i < size; ++i) {
        float xSeparation = (i - (size / 2)) * chosenPattern.droneSeparationHorizontal;
        
        result[i] = float3x3::RotateY(transform->GetGlobalRotation().ToEulerXZY().z) * float3(xSeparation + shiftOffset, 0.0f, chosenPattern.droneSeparationFromCenter);       // The value of Z is the correct angle since ToEulerXYZ decomposes it into 180,Y,180.
        result[i] += float3(0.0f, GetVerticalOffset(), 0.0f);
    }

    return result;
}

std::vector<float3> AttackDronesController::GenerateArrowFormation() {
    int size = dronesScripts.size();
    std::vector<float3> result(size);

    for (int i = 0; i < size / 2; ++i) {
        float xSeparation = -((size / 2) - i) * chosenPattern.droneSeparationHorizontal;
        float zSeparation = -((size / 2) - i) * chosenPattern.droneSeparationDepth;

        result[i] = float3x3::RotateY(transform->GetGlobalRotation().ToEulerXZY().z) * float3(xSeparation, 0.0f, zSeparation + chosenPattern.droneSeparationFromCenter);
        result[i] += float3(0.0f, GetVerticalOffset(), 0.0f);
    }

    for (int i = (size / 2); i < size; ++i) {
        float xSeparation = (i - (size / 2)) * chosenPattern.droneSeparationHorizontal;
        float zSeparation = -(i - (size / 2)) * chosenPattern.droneSeparationDepth;

        result[i] = float3x3::RotateY(transform->GetGlobalRotation().ToEulerXZY().z) * float3(xSeparation, 0.0f, zSeparation + chosenPattern.droneSeparationFromCenter);
        result[i] += float3(0.0f, GetVerticalOffset(), 0.0f);
    }

    return result;
}

std::vector<float3> AttackDronesController::GenerateCircleFormation() {
    int size = dronesScripts.size();
    std::vector<float3> result(size);

    for (int i = 0; i < size; ++i) {
        float theta = ((PI * 2) / size);
        float angle = (theta * i);
        
        result[i] = float3x3::RotateY(transform->GetGlobalRotation().ToEulerXZY().z) * (float3(cos(angle), 0.0f, sin(angle)) * chosenPattern.droneRadiusFormation);
        result[i] += float3(0.0f, GetVerticalOffset(), 0.0f);
    }

    return result;
}

float AttackDronesController::GetVerticalOffset() const {
    return transform->GetGlobalPosition().y + chosenPattern.droneVerticalOffset;
}

void AttackDronesController::StartWave() {
    if (dronesScripts.size() == 0) return;
    
    switch (cycle) {
        case WaveCycle::LEFT_TO_RIGHT: {
            float accumulatedDelay = 0.0f;
            for (AttackDroneBehavior* drone : dronesScripts) {
                drone->StartWave(chosenPattern.waves, accumulatedDelay, chosenPattern.timeBetweenWaves);
                accumulatedDelay += chosenPattern.droneShotDelay;
            }
            break;
        }

		case WaveCycle::RIGHT_TO_LEFT: {
			float accumulatedDelay = 0.0f;
			for (auto it = dronesScripts.rbegin(); it != dronesScripts.rend(); ++it) {
				(*it)->StartWave(chosenPattern.waves, accumulatedDelay, chosenPattern.timeBetweenWaves);
				accumulatedDelay += chosenPattern.droneShotDelay;
			}
			break;
		}

        case WaveCycle::CENTERED: {
            bool hasEvenDrones = dronesScripts.size() % 2 == 0;
            float orderedI = dronesScripts.size() / 2 - (hasEvenDrones ? 1 : 0);
            float accumulatedDelay = orderedI * chosenPattern.droneShotDelay;
            float multiplier = -1.0f;

            for (int i = 0; i < dronesScripts.size(); ++i) {
                dronesScripts[i]->StartWave(chosenPattern.waves, accumulatedDelay, chosenPattern.timeBetweenWaves);

                if (i == dronesScripts.size() / 2) {
                    multiplier = 1.0f;
                }

                if (hasEvenDrones && i == (dronesScripts.size() / 2) - 1) {
                    orderedI--;
                    multiplier = 1.0f;
                }

                orderedI += multiplier;
                accumulatedDelay = chosenPattern.droneShotDelay * orderedI;
            }
            break;
        }
    }
}
