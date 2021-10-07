#include "AttackDroneBehavior.h"

#include "GameplaySystems.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentParticleSystem.h"
#include "AttackDronesController.h"
#include "RandomNumberGenerator.h"

EXPOSE_MEMBERS(AttackDroneBehavior) {
    MEMBER(MemberType::GAME_OBJECT_UID, dronesControllerUID),
    MEMBER(MemberType::PREFAB_RESOURCE_UID, projectilePrefabUID),
    MEMBER(MemberType::FLOAT, droneSpeed),
    MEMBER(MemberType::FLOAT, droneRotationSpeed),
    MEMBER(MemberType::BOOL, droneMustHover),
    MEMBER(MemberType::FLOAT, droneHoverAmplitude),
    MEMBER(MemberType::BOOL, droneMustRecoil),
    MEMBER(MemberType::FLOAT, droneRecoilOffset),
    MEMBER(MemberType::FLOAT, droneRecoilTime),
};

GENERATE_BODY_IMPL(AttackDroneBehavior);

void AttackDroneBehavior::Start() {
    transform = GetOwner().GetComponent<ComponentTransform>();

    dronesController = GameplaySystems::GetGameObject(dronesControllerUID);
    if (dronesController) {
        dronesControllerTransform = dronesController->GetComponent<ComponentTransform>();
        dronesControllerScript = GET_SCRIPT(dronesController, AttackDronesController);
    }

    hoverCurrentTime = RandomNumberGenerator::GenerateFloat(-1.5708, 1.5708);
}

void AttackDroneBehavior::Update() {
    if (!transform || !dronesControllerTransform) return;

    transform->SetGlobalPosition(float3::Lerp(transform->GetGlobalPosition(), dronesControllerTransform->GetGlobalPosition() + positionOffset + GetHoverOffset() + GetRecoilOffset(), Time::GetDeltaTime() * droneSpeed));
    
    if (mustForceRotation) {
        float3 direction = (transform->GetGlobalPosition() - dronesControllerTransform->GetGlobalPosition()).Normalized();
        float3 right = Cross(float3(0, 1, 0), direction);
        direction = Cross(right, float3(0, 1, 0));

        transform->SetGlobalRotation(Quat::Lerp(transform->GetGlobalRotation(), Quat(float3x3(right, float3(0, 1, 0), direction)), Time::GetDeltaTime() * droneRotationSpeed));
    }
    else {
        transform->SetGlobalRotation(Quat::Lerp(transform->GetGlobalRotation(), dronesControllerTransform->GetGlobalRotation(), Time::GetDeltaTime() * droneRotationSpeed));
    }

    Shoot();
}

void AttackDroneBehavior::Reposition(float3 newPosition) {
    mustReposition = true;
}

void AttackDroneBehavior::SetPositionOffset(float3 newOffset) {
    positionOffset = newOffset;
}

void AttackDroneBehavior::SetMustForceRotation(bool mustForce) {
    mustForceRotation = mustForce;
}

void AttackDroneBehavior::SetWaitEndOfWave(bool mustWait) {
    mustWaitEndOfWave = mustWait;
    if (!mustWait) availableShot = true;
}

void AttackDroneBehavior::SetIsLastDrone(bool isLast) {
    isLastDrone = isLast;
}

void AttackDroneBehavior::SetControllerScript(AttackDronesController* controllerScript) {
    dronesControllerScript = controllerScript;
}

void AttackDroneBehavior::Shoot() {
    if (remainingWaves > 0 && availableShot) {
        if (currentTime >= delay) {
            remainingWaves--;
            shooter.Shoot(projectilePrefabUID, transform->GetGlobalPosition(), transform->GetGlobalRotation());
            currentTime = 0.0f;
            droneMustRecoil = true;
            if (mustWaitEndOfWave) availableShot = false;
            if (isLastDrone && dronesControllerScript) {
                isLastDrone = false;
                dronesControllerScript->EndOfWave();
            }
        }
        else {
            currentTime += Time::GetDeltaTime();
        }
    }
}

void AttackDroneBehavior::StartWave(int newWaves, float bulletDelay, float timeBetweenWaves) {
    remainingWaves = newWaves;
    currentTime = timeBetweenWaves - bulletDelay;     // Starts at the time between waves and will have to wait only for bulletDelay the first time
    delay = timeBetweenWaves;
}

float3 AttackDroneBehavior::GetHoverOffset() {
    float3 hoverPosition = float3(0.0f, 0.0f, 0.0f);

    if (droneMustHover) {
        hoverPosition = float3(0.0f, Sin(hoverCurrentTime) * droneHoverAmplitude, 0.f);
        hoverCurrentTime += Time::GetDeltaTime();
    }

    return hoverPosition;
}

float3 AttackDroneBehavior::GetRecoilOffset() {
    float3 recoilPosition = float3(0.0f, 0.0f, 0.0f);

   /* if (droneMustRecoil) {
        if (recoilCurrentTime > droneRecoilTime) {
            droneMustRecoil = false;
            recoilCurrentTime = 0.0f;
        }
        else {
            recoilPosition = float3::Lerp(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, droneRecoilOffset), recoilCurrentTime / droneRecoilTime);

            recoilCurrentTime += Time::GetDeltaTime();
        }
    }*/

    return recoilPosition;
}
