#include "AttackDroneBehavior.h"

#include "GameplaySystems.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentParticleSystem.h"

EXPOSE_MEMBERS(AttackDroneBehavior) {
    MEMBER(MemberType::GAME_OBJECT_UID, dronesControllerUID),
    MEMBER(MemberType::FLOAT, droneSpeed),
    MEMBER(MemberType::FLOAT, droneRotationSpeed),
    MEMBER(MemberType::PREFAB_RESOURCE_UID, projectilePrefabUID)
};

GENERATE_BODY_IMPL(AttackDroneBehavior);

void AttackDroneBehavior::Start() {
    transform = GetOwner().GetComponent<ComponentTransform>();

    dronesController = GameplaySystems::GetGameObject(dronesControllerUID);
    if (dronesController) {
        dronesControllerTransform = dronesController->GetComponent<ComponentTransform>();
    }
}

void AttackDroneBehavior::Update() {
    if (!transform || !dronesControllerTransform) return;

    transform->SetGlobalPosition(float3::Lerp(transform->GetGlobalPosition(), dronesControllerTransform->GetGlobalPosition() + positionOffset, Time::GetDeltaTime() * droneSpeed));
    
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
    currentPosition = newPosition;
}

void AttackDroneBehavior::SetPositionOffset(float3 newOffset) {
    positionOffset = newOffset;
}

void AttackDroneBehavior::SetMustForceRotation(bool mustForce) {
    mustForceRotation = mustForce;
}

void AttackDroneBehavior::Shoot() {
    if (remainingWaves > 0) {        
        if (currentTime >= delay) {
            remainingWaves--;
            shooter.Shoot(projectilePrefabUID, transform->GetGlobalPosition(), transform->GetGlobalRotation());
            currentTime = 0.0f;
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