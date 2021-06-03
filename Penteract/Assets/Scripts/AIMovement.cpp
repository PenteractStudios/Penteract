#include "AIMovement.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "TesseractEvent.h"

#include "PlayerController.h"
#include "HUDController.h"

#define MAX_ACCELERATION 9999

EXPOSE_MEMBERS(AIMovement) {
    MEMBER(MemberType::GAME_OBJECT_UID, playerUID),
    MEMBER(MemberType::GAME_OBJECT_UID, canvasUID),
    MEMBER(MemberType::INT, gruntCharacter.lifePoints),
    MEMBER(MemberType::FLOAT, gruntCharacter.movementSpeed),
    MEMBER(MemberType::INT, gruntCharacter.fallingSpeed),
    MEMBER(MemberType::FLOAT, gruntCharacter.searchRadius),
    MEMBER(MemberType::FLOAT, gruntCharacter.meleeRange),
    MEMBER(MemberType::FLOAT, gruntCharacter.timeToDie)
};

GENERATE_BODY_IMPL(AIMovement);

void AIMovement::Start() {
    player = GameplaySystems::GetGameObject(playerUID);
    agent = GetOwner().GetComponent<ComponentAgent>();
    if (agent) {
        agent->SetMaxSpeed(gruntCharacter.movementSpeed);
        agent->SetMaxAcceleration(MAX_ACCELERATION);
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

    if (hitTaken && gruntCharacter.lifePoints > 0) {
        gruntCharacter.lifePoints -= damageRecieved;
        hitTaken = false;
    }

    if (gruntCharacter.lifePoints <= 0) {
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
            Seek(float3(parentTransform->GetGlobalPosition().x, 0, parentTransform->GetGlobalPosition().z), gruntCharacter.fallingSpeed);
            if (parentTransform->GetGlobalPosition().y < 2.7 + 0e-5f) {
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
        Seek(player->GetComponent<ComponentTransform>()->GetGlobalPosition(), gruntCharacter.movementSpeed);
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

    if(!gruntCharacter.isAlive){
        if (gruntCharacter.timeToDie > 0) {
            gruntCharacter.timeToDie -= Time::GetDeltaTime();
        }
        else {
            if (hudControllerScript) {
                hudControllerScript->UpdateScore(10);
            }
            GameplaySystems::DestroyGameObject(&GetOwner());
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
        gruntCharacter.isAlive = false;
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
        return posTarget.Distance(parentTransform->GetGlobalPosition()) < gruntCharacter.searchRadius;
    }

    return false;
}

bool AIMovement::CharacterInMeleeRange(const GameObject* character)
{
    ComponentTransform* target = character->GetComponent<ComponentTransform>();
    if (target) {
        float3 posTarget = target->GetGlobalPosition();
        return posTarget.Distance(parentTransform->GetGlobalPosition()) < gruntCharacter.meleeRange;
    }

    return false;
}

void AIMovement::Seek(const float3& newPosition, float speed)
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
