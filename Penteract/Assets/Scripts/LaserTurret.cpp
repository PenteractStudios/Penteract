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
            currentState = TurretState::END;
        } else if (animationComp->GetCurrentState()->name == states[static_cast<int>(TurretState::END)]) {
            animationComp->SendTrigger(animationComp->GetCurrentState()->name + states[static_cast<int>(TurretState::START)]);
            currentState = TurretState::START;
        }

    }

}
