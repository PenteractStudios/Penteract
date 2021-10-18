#include "AttackDroneBehavior.h"

#include "GameplaySystems.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentParticleSystem.h"
#include "AttackDronesController.h"
#include "AttackDroneProjectile.h"
#include "RandomNumberGenerator.h"
#include "CurvesGenerator.h"

EXPOSE_MEMBERS(AttackDroneBehavior) {
    MEMBER_SEPARATOR("References"),
    MEMBER(MemberType::GAME_OBJECT_UID, dronesControllerUID),
    MEMBER(MemberType::PREFAB_RESOURCE_UID, projectilePrefabUID),
    MEMBER_SEPARATOR("Speed"),
    MEMBER(MemberType::FLOAT, droneSpeed),
    MEMBER(MemberType::FLOAT, droneSpeedOnRecoil),
    MEMBER(MemberType::FLOAT, droneRotationSpeed),
    MEMBER_SEPARATOR("Hover"),
    MEMBER(MemberType::BOOL, droneMustHover),
    MEMBER(MemberType::FLOAT, droneHoverAmplitude),
    MEMBER_SEPARATOR("Recoil"),
    MEMBER(MemberType::BOOL, droneMustRecoil),
    MEMBER(MemberType::FLOAT, droneRecoilTime),
    MEMBER(MemberType::FLOAT, droneRecoilDistance),
};

GENERATE_BODY_IMPL(AttackDroneBehavior);

void AttackDroneBehavior::Start() {
    transform = GetOwner().GetComponent<ComponentTransform>();

    dronesController = GameplaySystems::GetGameObject(dronesControllerUID);
    if (dronesController) {
        dronesControllerTransform = dronesController->GetComponent<ComponentTransform>();
        dronesControllerScript = GET_SCRIPT(dronesController, AttackDronesController);
    }

    GameObject* parent = GetOwner().GetParent();
    if (parent) {
        dronesContainerTransform = parent->GetComponent<ComponentTransform>();
    }

    hoverCurrentTime = RandomNumberGenerator::GenerateFloat(-1.5708, 1.5708);
}

void AttackDroneBehavior::Update() {
    if (!transform || !dronesControllerTransform || !dronesContainerTransform || droneDisabled) return;

    Translate();
    Rotate();
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
            GameObject* projectile = shooter.Shoot(projectilePrefabUID, transform->GetGlobalPosition(), transform->GetGlobalRotation());
            if (projectile) {
                AttackDroneProjectile* projectileScript = GET_SCRIPT(projectile, AttackDroneProjectile);
                if (projectileScript && dronesControllerScript) {
                    projectileScript->SetSpeed(dronesControllerScript->GetPatternProjectileSpeed());
                }
            }

            currentTime = 0.0f;
            if (droneMustRecoil) isRecoiling = true;
            if (mustWaitEndOfWave) availableShot = false;
            if (isLastDrone && dronesControllerScript) {
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

void AttackDroneBehavior::Deploy(float timeToReach) {
    isDeploying = true;
    isDismissing = false;
    deployTime = timeToReach;
    currentDeployTime = 0.0f;
    droneDisabled = false;
}

void AttackDroneBehavior::Dismiss(float timeToReach) {
    isDismissing = true;
    isDeploying = false;
    dismissTime = timeToReach;
    currentDismissTime = 0.0f;
}


void AttackDroneBehavior::Translate() {
    float3 targetPosition = dronesControllerTransform->GetGlobalPosition() + positionOffset + GetHoverOffset() + GetRecoilOffset();
    float progress = 0.0f;
    if (isDeploying) {
        progress = currentDeployTime / deployTime;
        if (currentDeployTime > deployTime) {
            isDeploying = false;
        }
        else {
            currentDeployTime += Time::GetDeltaTime();
        }
    }
    else if (isDismissing) {
        progress = currentDismissTime / dismissTime;
        targetPosition.y += 20.0f;

        if (currentDismissTime > dismissTime) {
            isDismissing = false;
            droneDisabled = true;
        }
        else {
            currentDismissTime += Time::GetDeltaTime();
        }
    }
    else {
        progress = Time::GetDeltaTime() * (isRecoiling ? droneSpeedOnRecoil : droneSpeed);
    }
    
    if (isDismissing) {
        transform->SetGlobalPosition(CurvesGenerator::SmoothStep7Float3(transform->GetGlobalPosition(), targetPosition, progress));
    }
    else if (isDeploying) {
        transform->SetGlobalPosition(CurvesGenerator::SmoothStep7Float3(transform->GetGlobalPosition(), targetPosition, progress));
    }
    else {
        transform->SetGlobalPosition(float3::Lerp(transform->GetGlobalPosition(), targetPosition, progress));
    }
}

void AttackDroneBehavior::Rotate() {
    if (mustForceRotation) {
        float3 direction = (transform->GetGlobalPosition() - dronesControllerTransform->GetGlobalPosition()).Normalized();
        float3 right = Cross(float3(0, 1, 0), direction);
        direction = Cross(right, float3(0, 1, 0));

        transform->SetGlobalRotation(Quat::Lerp(transform->GetGlobalRotation(), Quat(float3x3(right, float3(0, 1, 0), direction)), Time::GetDeltaTime() * droneRotationSpeed));
    }
    else {
        transform->SetGlobalRotation(Quat::Lerp(transform->GetGlobalRotation(), dronesControllerTransform->GetGlobalRotation(), Time::GetDeltaTime() * droneRotationSpeed));
    }
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

    if (droneMustRecoil && isRecoiling) {
        if (recoilCurrentTime > droneRecoilTime) {
            isRecoiling = false;
            recoilCurrentTime = 0.0f;
        }
        else {
            recoilPosition = float3::Lerp(float3(0.0f, 0.0f, 0.0f), -transform->GetFront() * droneRecoilDistance, recoilCurrentTime / droneRecoilTime);
            recoilPosition = float3(recoilPosition.x, 0.0f, recoilPosition.z);

            recoilCurrentTime += Time::GetDeltaTime();
        }
    }

    return recoilPosition;
}
