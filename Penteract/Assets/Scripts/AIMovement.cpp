#include "AIMovement.h"

#include "GameObject.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(AIMovement) {
    
};

GENERATE_BODY_IMPL(AIMovement);

void AIMovement::Start() {
    ownerTransform = GetOwner().GetComponent<ComponentTransform>();
    agent = GetOwner().GetComponent<ComponentAgent>();
}

void AIMovement::Update() {
    
}

void AIMovement::Seek(AIState state, const float3& newPosition, int speed)
{

    float3 position = ownerTransform->GetGlobalPosition();
    float3 direction = newPosition - position;

    velocity = direction.Normalized() * speed;

    position += velocity * Time::GetDeltaTime();

    if (state == AIState::START) {
        ownerTransform->SetGlobalPosition(position);
    }
    else {
        if (agent) {
            agent->SetMoveTarget(newPosition, true);
        }        
    }
    
    if (state != AIState::START) {
        Quat newRotation = Quat::LookAt(float3(0, 0, 1), direction.Normalized(), float3(0, 1, 0), float3(0, 1, 0));
        ownerTransform->SetGlobalRotation(newRotation);
    }
}

bool AIMovement::CharacterInSight(const GameObject* character, const float searchRadius)
{
    ComponentTransform* target = character->GetComponent<ComponentTransform>();
    if (target) {
        float3 posTarget = target->GetGlobalPosition();
        return posTarget.Distance(ownerTransform->GetGlobalPosition()) < searchRadius;
    }

    return false;
}

bool AIMovement::CharacterInMeleeRange(const GameObject* character, const float meleeRange)
{
    ComponentTransform* target = character->GetComponent<ComponentTransform>();
    if (target) {
        float3 posTarget = target->GetGlobalPosition();
        return posTarget.Distance(ownerTransform->GetGlobalPosition()) < meleeRange;
    }

    return false;
}
