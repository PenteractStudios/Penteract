#include "AttackDroneBehavior.h"

#include "GameplaySystems.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentParticleSystem.h"

EXPOSE_MEMBERS(AttackDroneBehavior) {
    MEMBER(MemberType::GAME_OBJECT_UID, dronesControllerUID),
    MEMBER(MemberType::FLOAT, droneSpeed),
    MEMBER(MemberType::PREFAB_RESOURCE_UID, projectilePrefabUID)
};

GENERATE_BODY_IMPL(AttackDroneBehavior);

void AttackDroneBehavior::Start() {
    transform = GetOwner().GetComponent<ComponentTransform>();
    //shooter = GetOwner().GetComponent<ComponentParticleSystem>();

    dronesController = GameplaySystems::GetGameObject(dronesControllerUID);
    if (dronesController) {
        dronesControllerTransform = dronesController->GetComponent<ComponentTransform>();
    }
}

void AttackDroneBehavior::Update() {
    if (!transform || !dronesControllerTransform) return;

    transform->SetGlobalPosition(float3::Lerp(transform->GetGlobalPosition(), dronesControllerTransform->GetGlobalPosition() + positionOffset, Time::GetDeltaTime() * droneSpeed));

    Shoot();
}

void AttackDroneBehavior::Reposition(float3 newPosition) {
    mustReposition = true;
    currentPosition = newPosition;
}

void AttackDroneBehavior::SetPositionOffset(float3 newOffset) {
    positionOffset = newOffset;
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
    Debug::Log(std::to_string(delay).c_str());
}