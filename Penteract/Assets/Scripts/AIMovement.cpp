#include "AIMovement.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "TesseractEvent.h"

#include "PlayerController.h"
#include "HUDController.h"

EXPOSE_MEMBERS(AIMovement) {
    MEMBER(MemberType::GAME_OBJECT_UID, playerUID),
    MEMBER(MemberType::GAME_OBJECT_UID, canvasUID),
    MEMBER(MemberType::INT, maxSpeed),
    MEMBER(MemberType::INT, lifePoints),
    MEMBER(MemberType::FLOAT, searchRadius),
    MEMBER(MemberType::FLOAT, meleeRange),
    MEMBER(MemberType::FLOAT, timeToDie)

};

GENERATE_BODY_IMPL(AIMovement);

void AIMovement::Start() {
    player = GameplaySystems::GetGameObject(playerUID);
    agent = GetOwner().GetComponent<ComponentAgent>();
    if (agent) {
        agent->SetMaxSpeed(maxSpeed);
        agent->SetMaxAcceleration(9999);
        agent->SetAgentObstacleAvoidance(true);
        agent->RemoveAgentFromCrowd();
    }
    animation = GetOwner().GetComponent<ComponentAnimation>();   
    parentTransform = GetOwner().GetComponent<ComponentTransform>();
    GameObject* canvas = GameplaySystems::GetGameObject(canvasUID);
    if (canvas) {
        hudControllerScript = GET_SCRIPT(canvas, HUDController);
    }
}

void AIMovement::Update() {
    if (!GetOwner().IsActive()) return;

    if (hitTaken && lifePoints > 0) {
        lifePoints -= damageRecieved;
        hitTaken = false;
    }

    if (lifePoints <= 0) {
        if (state == AIState::ATTACK) {
            animation->SendTrigger("AttackDeath");
        }
        else if (state == AIState::IDLE) {
            animation->SendTrigger("IdleDeath");
        }
        else if (state == AIState::RUN) {
            animation->SendTrigger("RunDeath");
        }
        Debug::Log("Death");
        agent->RemoveAgentFromCrowd();
        state = AIState::DEATH;
    }

    switch (state)
    {
    case AIState::START:
        if (Camera::CheckObjectInsideFrustum(GetOwner().GetChildren()[0])) {
            Seek(float3(parentTransform->GetGlobalPosition().x, 0, parentTransform->GetGlobalPosition().z), fallingSpeed);
            if (parentTransform->GetGlobalPosition().y < 2.7 + 0e-5f) {
                animation->SendTrigger("StartSpawn");
                state = AIState::SPAWN;
            }
        }
        break;
    case AIState::SPAWN:                
        break;
    case AIState::IDLE:
        PlayerController* playerController = GET_SCRIPT(player, PlayerController);
        if (!playerController.IsDead()) {
            if (CharacterInSight(player)) {
                animation->SendTrigger("IdleRun");
                state = AIState::RUN;
            }
        }
        break;
    case AIState::RUN:
        Seek(player->GetComponent<ComponentTransform>()->GetGlobalPosition(), maxSpeed);
        if (CharacterInMeleeRange(player)) {
            animation->SendTrigger("RunAttack");
            state = AIState::ATTACK;
        }
        break;
    case AIState::ATTACK:
        break;
    case AIState::DEATH:
        break;
    }

    if(dead){
        if (timeToDie > 0) {
            timeToDie -= Time::GetDeltaTime();
        }
        else {
            GameplaySystems::DestroyGameObject(&GetOwner());
            if (hudControllerScript) {
                hudControllerScript->UpdateScore(10);
            }
        }
    }
    	
}

void AIMovement::OnAnimationFinished()
{
    if (state == AIState::SPAWN) {
        animation->SendTrigger("SpawnIdle");
        state = AIState::IDLE;
        agent->AddAgentToCrowd();
    }
    
    else if(state == AIState::ATTACK)
    {
        PlayerController* playerController = GET_SCRIPT(player, PlayerController);
        playerController->HitDetected();
        animation->SendTrigger("AttackIdle");
        state = AIState::IDLE;
    }

    else if (state == AIState::DEATH) {
        dead = true;
    }
        
}

void AIMovement::HitDetected(int damage_) {
    damageRecieved = damage_;
    hitTaken = true;
}

bool AIMovement::CharacterInSight(const GameObject* character)
{
    ComponentTransform* target = character->GetComponent<ComponentTransform>();
    if (target) {
        float3 posTarget = target->GetGlobalPosition();
        return posTarget.Distance(parentTransform->GetGlobalPosition()) < searchRadius;
    }

    return false;
}

bool AIMovement::CharacterInMeleeRange(const GameObject* character)
{
    ComponentTransform* target = character->GetComponent<ComponentTransform>();
    if (target) {
        float3 posTarget = target->GetGlobalPosition();
        return posTarget.Distance(parentTransform->GetGlobalPosition()) < meleeRange;
    }

    return false;
}

void AIMovement::Seek(const float3& newPosition, int speed)
{

    float3 position = parentTransform->GetGlobalPosition();
    float3 direction = newPosition - position;

    velocity = direction.Normalized() * speed;

    position += velocity * Time::GetDeltaTime();

    if (state == AIState::START) {
        parentTransform->SetGlobalPosition(position);
    }
    else {
        agent->SetMoveTarget(newPosition, true);
    }
    
    if (state != AIState::START) {
        Quat newRotation = Quat::LookAt(float3(0, 0, 1), direction.Normalized(), float3(0, 1, 0), float3(0, 1, 0));
        parentTransform->SetGlobalRotation(newRotation);
    }
}
