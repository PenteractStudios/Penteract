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
    MEMBER(MemberType::GAME_OBJECT_UID, laserWarningUID),
    MEMBER(MemberType::GAME_OBJECT_UID, pairGeneratorUID)
};

GENERATE_BODY_IMPL(BossLaserGenerator);

void BossLaserGenerator::Start() {
	GameObject* ownerGo = &GetOwner();
    GameObject* pair = GameplaySystems::GetGameObject(pairGeneratorUID);

    if (ownerGo) {
        animationComp = ownerGo->GetComponent<ComponentAnimation>();
        audioComp = ownerGo->GetComponent<ComponentAudioSource>();
    }
    if (pair) {
        pairAnimationComp = pair->GetComponent<ComponentAnimation>();
        pairAudioComp = pair->GetComponent<ComponentAudioSource>();
        pairScript = GET_SCRIPT(pair, BossLaserGenerator);
    }

    laserObject = GameplaySystems::GetGameObject(laserTargetUID);
    if (laserObject) {
        laserAudio = laserObject->GetComponent<ComponentAudioSource>();
        laserObject->Disable();
    }

    laserWarning = GameplaySystems::GetGameObject(laserWarningUID);
    if (laserWarning) {
        laserWarningAudio = laserWarning->GetComponent<ComponentAudioSource>();
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
                    if (laserWarningVFX) {
                        laserWarningVFX->PlayChildParticles();
                        if (laserWarningAudio) laserWarningAudio->Play();
                    }
                }
                if(pairScript) pairScript->beingUsed = true;
                if (!beingUsed) {
                    animationComp->SendTrigger(states[static_cast<unsigned int>(GeneratorState::IDLE)] + states[static_cast<unsigned int>(GeneratorState::START)]);
                    if (audioComp && !audioComp->IsPlaying()) audioComp->Play();
                }
                if (pairScript && pairScript->currentState == GeneratorState::IDLE) {
                    pairAnimationComp->SendTrigger(states[static_cast<unsigned int>(GeneratorState::IDLE)] + states[static_cast<unsigned int>(GeneratorState::START)]);
                    if (pairAudioComp && !pairAudioComp->IsPlaying()) pairAudioComp->Play();
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
                if(pairScript) pairScript->beingUsed = false;
                if (!beingUsed) {
                    animationComp->SendTrigger(states[static_cast<unsigned int>(GeneratorState::SHOOT)] + states[static_cast<unsigned int>(GeneratorState::IDLE)]);
                    if (audioComp && audioComp->IsPlaying()) audioComp->Stop();
                }
                if (pairScript && pairScript->currentState == GeneratorState::IDLE) {
                    pairAnimationComp->SendTrigger(states[static_cast<unsigned int>(GeneratorState::SHOOT)] + states[static_cast<unsigned int>(GeneratorState::IDLE)]);
                    if (pairAudioComp && pairAudioComp->IsPlaying()) pairAudioComp->Stop();
                }
            }
        }
        break;
    }
    if (currentState != GeneratorState::SHOOT) {
        if (laserObject && laserObject->IsActive()) { 
            if (laserAudio) laserAudio->Stop();
            laserObject->Disable(); 
        }
    }
    else {
        if (laserObject && !laserObject->IsActive()) {
            laserObject->Enable();
            if (laserAudio) laserAudio->Play();
        }
    }
}

void BossLaserGenerator::Init() {
    currentState = GeneratorState::IDLE;
}

void BossLaserGenerator::StopAudio() {
    if (laserAudio) laserAudio->Stop();
    if (audioComp) audioComp->Stop();
    if (pairAudioComp) pairAudioComp->Stop();
    if (laserWarningAudio) laserWarningAudio->Stop();
}