#include "BossLaserGenerator.h"

#include "GameplaySystems.h"
#include "GameObject.h"

EXPOSE_MEMBERS(BossLaserGenerator) {
    MEMBER(MemberType::FLOAT, coolDownOn),
    MEMBER(MemberType::FLOAT, coolDownOnTimer),
    MEMBER(MemberType::FLOAT, coolDownOff),
    MEMBER(MemberType::FLOAT, coolDownOffTimer),
    MEMBER(MemberType::FLOAT, chargingDuration),
    MEMBER(MemberType::GAME_OBJECT_UID, laserTargetUID),
    MEMBER(MemberType::GAME_OBJECT_UID, laserWarningUID)
};

GENERATE_BODY_IMPL(BossLaserGenerator);

void BossLaserGenerator::Start() {
	GameObject* ownerGo = &GetOwner();

    if (ownerGo) {
        animationComp = ownerGo->GetComponent<ComponentAnimation>();
    }

    laserObject = GameplaySystems::GetGameObject(laserTargetUID);
    if (laserObject) {
        laserObject->Disable();
    }

    laserWarning = GameplaySystems::GetGameObject(laserWarningUID);
    if (laserWarning) {
        laserWarning->Disable();
    }
}

void BossLaserGenerator::Update() {
	if (!animationComp) return;

    switch (currentState) {
    case GeneratorState::IDLE:
        if (coolDownOffTimer <= coolDownOff) {
            coolDownOffTimer += Time::GetDeltaTime();
            if (coolDownOffTimer > coolDownOff) {
                coolDownOffTimer = 0;
                currentState = GeneratorState::START;
                if (laserWarning) {
                    laserWarning->Enable();
                    ComponentParticleSystem* laserWarningVFX = laserWarning->GetComponent<ComponentParticleSystem>();
                    if (laserWarningVFX) laserWarningVFX->PlayChildParticles();
                }
                if (!beingUsed) {
                    animationComp->SendTrigger(states[static_cast<unsigned int>(GeneratorState::IDLE)] + states[static_cast<unsigned int>(GeneratorState::START)]);
                }
            }
        }
        break;
    case GeneratorState::START:
        if (chargingTimer <= chargingDuration) {
            chargingTimer += Time::GetDeltaTime();
            if (chargingTimer > chargingDuration) {
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
                if (!beingUsed) {
                    animationComp->SendTrigger(states[static_cast<unsigned int>(GeneratorState::SHOOT)] + states[static_cast<unsigned int>(GeneratorState::IDLE)]);
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

void BossLaserGenerator::Init() {
    currentState = GeneratorState::IDLE;
}