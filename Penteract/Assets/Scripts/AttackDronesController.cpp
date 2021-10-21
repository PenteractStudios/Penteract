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

    MEMBER_SEPARATOR("Delay until drones are positioned"),
    MEMBER(MemberType::FLOAT, dronesRepositionDelay),

    MEMBER_SEPARATOR("Delay until drones start shooting"),
    MEMBER(MemberType::FLOAT, dronesStartShootingDelay),

    MEMBER_SEPARATOR("Delay until drones start leaving"),
    MEMBER(MemberType::FLOAT, dronesDismissDelay)
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
            droneScript->SetControllerScript(this);
        }
    }

    RecalculateFormations();
    SetDronesFormation(formation);
}

void AttackDronesController::Update() {
    if (!currentPosition.Equals(transform->GetGlobalPosition()) || !currentRotation.Equals(transform->GetGlobalRotation())) {
        currentPosition = transform->GetGlobalPosition();
        currentRotation = transform->GetGlobalRotation();

        RecalculateFormations();
        RepositionDrones();
    }

    if (mustStartBulletHell) {
        if (currentTime > dronesRepositionDelay + dronesStartShootingDelay) {
            StartWave();
            mustStartBulletHell = false;
            currentTime = 0.0f;
        }
        else {
            currentTime += Time::GetDeltaTime();
        }
    }

    if (mustStopBulletHell) {
        if (currentTime > dronesDismissDelay) {
            DismissDrones();
            mustStopBulletHell = false;
            currentTime = 0.0f;
        }
        else {
            currentTime += Time::GetDeltaTime();
        }
    }

    if (BulletHellActive() && chosenPattern.rotationDronesActive) {
        rotationOffset += (chosenPattern.rotationDronesClockwise ? 1 : -1) * Time::GetDeltaTime() * chosenPattern.rotationDronesSpeed;

        RecalculateFormations();
        RepositionDrones();
    }
}

void AttackDronesController::StartBulletHell() {
    if (patterns.empty()) return;

    int chance = RandomNumberGenerator::GenerateInteger(1, 100);
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
    
    if (newNumberOfDrones == 0) return;

    waves = 1;
    cycle = chosenPattern.cycles[0];
    mustStartBulletHell = true;
    mustStopBulletHell = false;
    currentTime = 0.0f;
    rotationOffset = 0.0f;
    bulletHellFinished = false;

    CheckDronesWaitEndOfWave();
    RecalculateFormations();
    SetDronesFormation(chosenPattern.droneFormation);
    DeployDrones();
}

void AttackDronesController::EndOfWave() {
    waves++;
    if (waves > chosenPattern.waves) {
        bulletHellFinished = true;
        mustStopBulletHell = true;
    }
    if (waves > chosenPattern.waves || !HadToWaitEndOfWave()) return;
    
    cycle = chosenPattern.cycles[waves - 1];
    CheckDronesWaitEndOfWave();
    StartWave();
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

void AttackDronesController::DeployDrones() {
    for (AttackDroneBehavior* drone : dronesScripts) {
        drone->Deploy(dronesRepositionDelay);
    }
}

void AttackDronesController::DismissDrones() {
    for (AttackDroneBehavior* drone : dronesScripts) {
        drone->Dismiss(dronesRepositionDelay);
    }
}

void AttackDronesController::RecalculateFormations() {
    if (dronesScripts.size() == 0) return;

    formationsOffsetPositions.clear();

    switch (chosenPattern.droneFormation) {
        case DronesFormation::LINE:
            formationsOffsetPositions = GenerateLineFormation();
            break;
        case DronesFormation::ARROW:
            formationsOffsetPositions = GenerateArrowFormation();
            break;
        case DronesFormation::CIRCLE:
            formationsOffsetPositions = GenerateCircleFormation();
            break;
    }
}

void AttackDronesController::RepositionDrones() {
    for (int i = 0; i < dronesScripts.size(); ++i) {
        dronesScripts[i]->SetPositionOffset(formationsOffsetPositions[i]);
        dronesScripts[i]->SetMustForceRotation(cycle == WaveCycle::CENTERED && formation == DronesFormation::CIRCLE);
    }
}

void AttackDronesController::CheckDronesWaitEndOfWave() {
    SetDronesWaitEndOfWave(false);      // clear and initialize

    if (chosenPattern.cycles.size() > 1 && waves <= chosenPattern.cycles.size()) {
        bool mustWaitEndOfWave = MustWaitEndOfWave();
        if (mustWaitEndOfWave) {
            SetDronesWaitEndOfWave(true);
        }
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
        float angle = (theta * i) + rotationOffset;
        
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

    float maxDelay = 0.0f;
    int mostDelayedDrone = 0;
    float delayWaitWave = HadToWaitEndOfWave() ? chosenPattern.timeBetweenWaves : 0.0f;
    
    switch (cycle) {
        case WaveCycle::LEFT_TO_RIGHT: {
            mostDelayedDrone = dronesScripts.size() - 1;
            float accumulatedDelay = 0.0f + delayWaitWave;

            for (int i = 0; i < dronesScripts.size(); ++i) {
                if (accumulatedDelay >= maxDelay) {
                    maxDelay = accumulatedDelay;
                    mostDelayedDrone = i;
                }

                dronesScripts[i]->StartWave(chosenPattern.waves + 1 - waves, accumulatedDelay, chosenPattern.timeBetweenWaves);
                accumulatedDelay += chosenPattern.droneShotDelay;
            }
            break;
        }

		case WaveCycle::RIGHT_TO_LEFT: {
            mostDelayedDrone = 0;
			float accumulatedDelay = 0.0f + delayWaitWave;

            for (int i = dronesScripts.size() - 1; i >= 0; --i) {
                if (accumulatedDelay >= maxDelay) {
                    maxDelay = accumulatedDelay;
                    mostDelayedDrone = i;
                }

                dronesScripts[i]->StartWave(chosenPattern.waves + 1 - waves, accumulatedDelay, chosenPattern.timeBetweenWaves);
                accumulatedDelay += chosenPattern.droneShotDelay;
            }

			break;
		}

        case WaveCycle::CENTERED: {
            mostDelayedDrone = dronesScripts.size() - 1;

            bool hasEvenDrones = dronesScripts.size() % 2 == 0;
            float orderedI = dronesScripts.size() / 2 - (hasEvenDrones ? 1 : 0);
            float accumulatedDelay = (orderedI * chosenPattern.droneShotDelay) + delayWaitWave;
            float multiplier = -1.0f;

            for (int i = 0; i < dronesScripts.size(); ++i) {
                if (accumulatedDelay >= maxDelay) {
                    maxDelay = accumulatedDelay;
                    mostDelayedDrone = i;
                }

                dronesScripts[i]->StartWave(chosenPattern.waves + 1 - waves, accumulatedDelay, chosenPattern.timeBetweenWaves);

                if (i == dronesScripts.size() / 2) {
                    multiplier = 1.0f;
                }

                if (hasEvenDrones && i == (dronesScripts.size() / 2) - 1) {
                    orderedI--;
                    multiplier = 1.0f;
                }

                orderedI += multiplier;
                accumulatedDelay = (chosenPattern.droneShotDelay * orderedI) + delayWaitWave;
            }
            break;
        }
    }

    SetDronesIsLastDrone(false);
    dronesScripts[mostDelayedDrone]->SetIsLastDrone(true);
}

void AttackDronesController::SetDronesWaitEndOfWave(bool value) {
    for (AttackDroneBehavior* drone : dronesScripts) {
        drone->SetWaitEndOfWave(value);
    }
}

void AttackDronesController::SetDronesIsLastDrone(bool value) {
    for (AttackDroneBehavior* drone : dronesScripts) {
        drone->SetIsLastDrone(value);
    }
}

bool AttackDronesController::MustWaitEndOfWave() const {
    if (waves == chosenPattern.waves) return true;
    return chosenPattern.cycles[waves - 1] != chosenPattern.cycles[waves];
}

bool AttackDronesController::HadToWaitEndOfWave() const {
    if (waves > 1 && waves <= chosenPattern.cycles.size()) {
        if (chosenPattern.cycles[waves - 1] != chosenPattern.cycles[waves - 2]) {           // If current 
            return true;
        }
    }
    return false;
}

bool AttackDronesController::BulletHellActive() const {
    return !bulletHellFinished && waves <= chosenPattern.waves;
}

bool AttackDronesController::BulletHellFinished() const {
    return bulletHellFinished;
}

bool AttackDronesController::IsBulletHellCircular() const {
    return chosenPattern.droneFormation == DronesFormation::CIRCLE;
}

float AttackDronesController::GetPatternProjectileSpeed() const {
    return chosenPattern.droneProjectileSpeed;
}
