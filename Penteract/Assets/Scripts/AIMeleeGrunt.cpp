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
    if (player) {
        playerController = GET_SCRIPT(player, PlayerController);
    }
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
    if (!player) return;
    if (!agent) return;
    if (!movementScript) return;
    if (!hudControllerScript) return;
    if (!playerController) return;
    if (!ownerTransform) return;
    if (!animation) return;

    if (hitTaken && lifePoints > 0) {
        lifePoints -= damageRecieved;
        hitTaken = false;
    }

    if (lifePoints <= 0) {
        if (state == AIState::ATTACK) {
            animation->SendTrigger("RunDeath");
            animation->SendTriggerSecondary("AttackDeath");
        }
        else if (state == AIState::IDLE) {
            animation->SendTrigger("IdleDeath");
            animation->SendTriggerSecondary("IdleDeath");
        }
        else if (state == AIState::RUN) {
            animation->SendTrigger("RunDeath");
            animation->SendTriggerSecondary("RunDeath");
        }
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
        if (!playerController->IsDead()) {
            if (movementScript->CharacterInSight(player, searchRadius)) {
                animation->SendTrigger("IdleRun");
                state = AIState::RUN;
            }
        }
        break;
    case AIState::RUN:
        movementScript->Seek(state, player->GetComponent<ComponentTransform>()->GetGlobalPosition(), maxSpeed);
        if (movementScript->CharacterInMeleeRange(player, meleeRange)) {
            animation->SendTriggerSecondary("RunAttack");
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

    else if (state == AIState::DEATH) {
        dead = true;
    }

}

void AIMeleeGrunt::OnAnimationSecondaryFinished()
{
    if (state == AIState::ATTACK)
    {
        playerController->HitDetected();
        animation->SendTriggerSecondary("Attack" + animation->GetCurrentState()->name);
        state = AIState::IDLE;
    }
}

void AIMeleeGrunt::HitDetected(int damage_) {
    damageRecieved = damage_;
    hitTaken = true;
}
