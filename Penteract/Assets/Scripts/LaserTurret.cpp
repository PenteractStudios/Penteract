#include "LaserTurret.h"

#include "GameplaySystems.h"
#include "GameObject.h"

EXPOSE_MEMBERS(LaserTurret) {
    MEMBER(MemberType::GAME_OBJECT_UID, laserTargetUID)
};

GENERATE_BODY_IMPL(LaserTurret);

void LaserTurret::Start() {
	
    GameObject* owner = &GetOwner();

    if (owner) {
        animationComp = owner->GetComponent<ComponentAnimation>();
    }

    laserObject = GameplaySystems::GetGameObject(laserTargetUID);
    if (laserObject) {
        laserObject->Disable();
    }

}

void LaserTurret::Update() {

    if (!animationComp) {
        Debug::Log("animationComp is nullptr");
    }

    switch (currentState)
    {
    case TurretState::IDLE_START:
        if (coolDownOff < coolDownOffTimer) {
            coolDownOff += Time::GetDeltaTime();
            if (coolDownOff > coolDownOffTimer) {
                coolDownOff = 0;
                currentState = TurretState::START;
                animationComp->SendTrigger(states[static_cast<unsigned int>(TurretState::IDLE_START)] + states[static_cast<unsigned int>(TurretState::START)]);
            }
        }
        break;
    case TurretState::IDLE_END:
        if (coolDownOn < coolDownOnTimer) {
            coolDownOn += Time::GetDeltaTime();
            if (coolDownOff > coolDownOffTimer) {
                coolDownOff = 0;
                currentState = TurretState::END;
                animationComp->SendTrigger(states[static_cast<unsigned int>(TurretState::IDLE_END)] + states[static_cast<unsigned int>(TurretState::END)]);
            }
        }
        break;
    }

    if (currentState != TurretState::SHOOT) {
        if (laserObject && laserObject->IsActive()) laserObject->Disable();
    }
    else {
        if (laserObject && !laserObject->IsActive()) laserObject->Enable();
    }
	
}

void LaserTurret::Init() {

    currentState = TurretState::START;

}

void LaserTurret::OnAnimationFinished() {

    if (animationComp && animationComp->GetCurrentState()) {

        if (animationComp->GetCurrentState()->name == states[static_cast<int>(TurretState::START)]) {
            animationComp->SendTrigger(animationComp->GetCurrentState()->name + states[static_cast<int>(TurretState::SHOOT)]);
            currentState = TurretState::SHOOT;
        } else if (animationComp->GetCurrentState()->name == states[static_cast<int>(TurretState::SHOOT)]) {
            animationComp->SendTrigger(animationComp->GetCurrentState()->name + states[static_cast<int>(TurretState::END)]);
            currentState = TurretState::IDLE_END;
        } else if (animationComp->GetCurrentState()->name == states[static_cast<int>(TurretState::END)]) {
            animationComp->SendTrigger(animationComp->GetCurrentState()->name + states[static_cast<int>(TurretState::START)]);
            currentState = TurretState::IDLE_START;
        }

    }

}
