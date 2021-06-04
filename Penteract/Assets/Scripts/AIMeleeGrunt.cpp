#include "AIMeleeGrunt.h"

#include "GameObject.h"
#include "GameplaySystems.h"

#include "PlayerController.h"
#include "HUDController.h"
#include "AIMovement.h"

EXPOSE_MEMBERS(AIMeleeGrunt) {
    MEMBER(MemberType::GAME_OBJECT_UID, playerUID),
        MEMBER(MemberType::GAME_OBJECT_UID, canvasUID),
        MEMBER(MemberType::INT, maxSpeed),
        MEMBER(MemberType::INT, lifePoints),
        MEMBER(MemberType::FLOAT, searchRadius),
        MEMBER(MemberType::FLOAT, meleeRange),
        MEMBER(MemberType::FLOAT, timeToDie)

};

GENERATE_BODY_IMPL(AIMeleeGrunt);

void AIMeleeGrunt::Start() {
    player = GameplaySystems::GetGameObject(playerUID);
    agent = GetOwner().GetComponent<ComponentAgent>();
    if (agent) {
        agent->SetMaxSpeed(maxSpeed);
        agent->SetMaxAcceleration(9999);
        agent->SetAgentObstacleAvoidance(true);
        agent->RemoveAgentFromCrowd();
    }
    animation = GetOwner().GetComponent<ComponentAnimation>();
    ownerTransform = GetOwner().GetComponent<ComponentTransform>();
    GameObject* canvas = GameplaySystems::GetGameObject(canvasUID);
    if (canvas) {
        hudControllerScript = GET_SCRIPT(canvas, HUDController);
    }
    movementScript = GET_SCRIPT(&GetOwner(), AIMovement);
}

void AIMeleeGrunt::Update() {
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
            movementScript->Seek(state, float3(ownerTransform->GetGlobalPosition().x, 0, ownerTransform->GetGlobalPosition().z), fallingSpeed);
            if (ownerTransform->GetGlobalPosition().y < 2.7 + 0e-5f) {
                animation->SendTrigger("StartSpawn");
                state = AIState::SPAWN;
            }
        }
        break;
    case AIState::SPAWN:
        break;
    case AIState::IDLE:
        if (player) {
            if (CharacterInSight(player)) {
                animation->SendTrigger("IdleRun");
                state = AIState::RUN;
            }
        }
        break;
    case AIState::RUN:
        movementScript->Seek(state, player->GetComponent<ComponentTransform>()->GetGlobalPosition(), maxSpeed);
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

    if (dead) {
        if (timeToDie > 0) {
            timeToDie -= Time::GetDeltaTime();
        }
        else {
            if (hudControllerScript) {
                hudControllerScript->UpdateScore(10);
            }
            GameplaySystems::DestroyGameObject(&GetOwner());
        }
    }

}

void AIMeleeGrunt::OnAnimationFinished()
{
    if (state == AIState::SPAWN) {
        animation->SendTrigger("SpawnIdle");
        state = AIState::IDLE;
        agent->AddAgentToCrowd();
    }

    else if (state == AIState::ATTACK)
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

void AIMeleeGrunt::HitDetected(int damage_) {
    damageRecieved = damage_;
    hitTaken = true;
}

bool AIMeleeGrunt::CharacterInSight(const GameObject* character)
{
    ComponentTransform* target = character->GetComponent<ComponentTransform>();
    if (target) {
        float3 posTarget = target->GetGlobalPosition();
        return posTarget.Distance(ownerTransform->GetGlobalPosition()) < searchRadius;
    }

    return false;
}

bool AIMeleeGrunt::CharacterInMeleeRange(const GameObject* character)
{
    ComponentTransform* target = character->GetComponent<ComponentTransform>();
    if (target) {
        float3 posTarget = target->GetGlobalPosition();
        return posTarget.Distance(ownerTransform->GetGlobalPosition()) < meleeRange;
    }

    return false;
}
