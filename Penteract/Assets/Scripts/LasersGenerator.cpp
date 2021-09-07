#include "LasersGenerator.h"

#include "GameplaySystems.h"
#include "GameObject.h"

EXPOSE_MEMBERS(LasersGenerator) {
    MEMBER(MemberType::FLOAT, coolDownOn),
    MEMBER(MemberType::FLOAT, coolDownOnTimer),
    MEMBER(MemberType::FLOAT, coolDownOff),
    MEMBER(MemberType::FLOAT, coolDownOffTimer),
    MEMBER(MemberType::GAME_OBJECT_UID, laserTargetUID),
    MEMBER(MemberType::GAME_OBJECT_UID, pairGeneratorUID)
};

GENERATE_BODY_IMPL(LasersGenerator);

void LasersGenerator::Start() {
    GameObject* owner = &GetOwner();
    GameObject* pair = GameplaySystems::GetGameObject(pairGeneratorUID);

    if (owner) {
        animationComp = owner->GetComponent<ComponentAnimation>();
    }
    if (pair) {
        pairAnimationComp = pair->GetComponent<ComponentAnimation>();
        pairScript = GET_SCRIPT(pair, LasersGenerator);
    }

    laserObject = GameplaySystems::GetGameObject(laserTargetUID);
    if (laserObject) {
        laserObject->Disable();
    }
}

void LasersGenerator::Update() {
    if (!animationComp) return;

    switch (currentState)
    {
    case GeneratorState::IDLE:
        if (coolDownOffTimer <= coolDownOff) {
            coolDownOffTimer += Time::GetDeltaTime();
            if (coolDownOffTimer > coolDownOff) {
                coolDownOffTimer = 0;
                currentState = GeneratorState::START;
                pairScript->beingUsed = true;
                if (!beingUsed) {
                    animationComp->SendTrigger(states[static_cast<unsigned int>(GeneratorState::IDLE)] + states[static_cast<unsigned int>(GeneratorState::START)]);
                }
                if (pairScript->currentState == GeneratorState::IDLE) {
                    pairAnimationComp->SendTrigger(states[static_cast<unsigned int>(GeneratorState::IDLE)] + states[static_cast<unsigned int>(GeneratorState::START)]);
                }
            }
        }
        break;
    case GeneratorState::START:
        if (chargingTimer <= CHARGING_DURATION) {
            chargingTimer += Time::GetDeltaTime();
            if (chargingTimer > CHARGING_DURATION) {
                chargingTimer = 0.f;
                currentState = GeneratorState::SHOOT;
            }
        }
        break;
    case GeneratorState::SHOOT:
        if (coolDownOnTimer <= coolDownOn) {
            coolDownOnTimer += Time::GetDeltaTime();
            if (coolDownOnTimer > coolDownOn) {
                coolDownOnTimer = 0;
                currentState = GeneratorState::IDLE;
                pairScript->beingUsed = false;
                if (!beingUsed) {
                    animationComp->SendTrigger(states[static_cast<unsigned int>(GeneratorState::SHOOT)] + states[static_cast<unsigned int>(GeneratorState::IDLE)]);
                }
                if (pairScript->currentState == GeneratorState::IDLE) {
                    pairAnimationComp->SendTrigger(states[static_cast<unsigned int>(GeneratorState::SHOOT)] + states[static_cast<unsigned int>(GeneratorState::IDLE)]);
                }
            }
        }
        break;
    }

    if (currentState != GeneratorState::SHOOT) {
        if (laserObject && laserObject->IsActive()) laserObject->Disable();
    }
    else {
        if (laserObject && !laserObject->IsActive()) laserObject->Enable();
    }
}

void LasersGenerator::Init() {
    currentState = GeneratorState::IDLE;
}