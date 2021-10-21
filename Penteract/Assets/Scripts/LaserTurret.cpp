#include "LaserTurret.h"

#include "GameplaySystems.h"
#include "GameObject.h"

EXPOSE_MEMBERS(LaserTurret) {
    MEMBER(MemberType::FLOAT, coolDownOn),
    MEMBER(MemberType::FLOAT, coolDownOnTimer),
    MEMBER(MemberType::FLOAT, coolDownOff),
    MEMBER(MemberType::FLOAT, coolDownOffTimer),
    MEMBER(MemberType::GAME_OBJECT_UID, laserTargetUID),
    MEMBER(MemberType::GAME_OBJECT_UID, laserWarningUID)
};

GENERATE_BODY_IMPL(LaserTurret);

void LaserTurret::Start() {

    GameObject* ownerGo = &GetOwner();

    if (ownerGo) {
        animationComp = ownerGo->GetComponent<ComponentAnimation>();
    }

    laserObject = GameplaySystems::GetGameObject(laserTargetUID);
    if (laserObject) {
        laserAudio = laserObject->GetComponent<ComponentAudioSource>();
        laserObject->Disable();
    }

    laserWarning = GameplaySystems::GetGameObject(laserWarningUID);
    if (laserWarning) {
        laserWarning->Disable();
    }

    int i = 0;
    for (ComponentAudioSource& src : GetOwner().GetComponents<ComponentAudioSource>()) {
        if (i < static_cast<int>(Audios::TOTAL)) audios[i] = &src;
        i++;
    }
}

void LaserTurret::Update() {

    if (!animationComp) return;

    switch (currentState)
    {
    case TurretState::IDLE_START:
        if (coolDownOffTimer <= coolDownOff) {
            coolDownOffTimer += Time::GetDeltaTime();
            if (coolDownOffTimer > coolDownOff) {
                coolDownOffTimer = 0;
                currentState = TurretState::START;
                if (laserWarning) {
                    laserWarning->Enable();
                    ComponentParticleSystem* laserWarningVFX = laserWarning->GetComponent<ComponentParticleSystem>();
                    ComponentAudioSource* audioWarning = laserWarning->GetComponent<ComponentAudioSource>();
                    if (laserWarningVFX) {
                        laserWarningVFX->PlayChildParticles();
                        if (audioWarning) audioWarning->Play();
                    }
                }
                animationComp->SendTrigger(states[static_cast<unsigned int>(TurretState::IDLE_START)] + states[static_cast<unsigned int>(TurretState::START)]);

                if (audios[static_cast<int>(Audios::UP)]) audios[static_cast<int>(Audios::UP)]->Play();
            }
        }
        break;
    case TurretState::SHOOTING_END:
        if (coolDownOnTimer <= coolDownOn) {
            coolDownOnTimer += Time::GetDeltaTime();
            if (coolDownOnTimer > coolDownOn) {
                coolDownOnTimer = 0;
                currentState = TurretState::END;
                animationComp->SendTrigger(states[static_cast<unsigned int>(TurretState::SHOOTING_END)] + states[static_cast<unsigned int>(TurretState::END)]);

                if (audios[static_cast<int>(Audios::DOWN)]) audios[static_cast<int>(Audios::DOWN)]->Play();
            }
        }
        break;
    }

    if (currentState != TurretState::SHOOT && currentState != TurretState::SHOOTING_END) {
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

void LaserTurret::Init() {

    currentState = TurretState::IDLE_START;

}

void LaserTurret::OnAnimationFinished() {

    if (animationComp && animationComp->GetCurrentState()) {

        if (animationComp->GetCurrentState()->name == states[static_cast<int>(TurretState::START)]) {
            animationComp->SendTrigger(animationComp->GetCurrentState()->name + states[static_cast<int>(TurretState::SHOOT)]);
            currentState = TurretState::SHOOT;
            coolDownOnTimer = 0.0f;
            coolDownOffTimer = 0.0f;
        } else if (animationComp->GetCurrentState()->name == states[static_cast<int>(TurretState::SHOOT)]) {
            currentState = TurretState::SHOOTING_END;
            coolDownOnTimer = 0.0f;
            animationComp->SendTrigger(animationComp->GetCurrentState()->name + states[static_cast<int>(TurretState::SHOOTING_END)]);
        } else if (animationComp->GetCurrentState()->name == states[static_cast<int>(TurretState::END)]) {
            currentState = TurretState::IDLE_START;
            coolDownOffTimer = 0.0f;
            animationComp->SendTrigger(animationComp->GetCurrentState()->name + states[static_cast<int>(TurretState::IDLE_START)]);
        }

    }

}
