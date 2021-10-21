#include "MovingLasers.h"

#include "GameplaySystems.h"
#include "GameObject.h"

EXPOSE_MEMBERS(MovingLasers) {
    MEMBER(MemberType::FLOAT, chargingDuration),
    MEMBER(MemberType::FLOAT, movementSpeed),
    MEMBER(MemberType::FLOAT3, initialGeneratorPosition),
    MEMBER(MemberType::FLOAT3, finalGeneratorPosition),
    MEMBER(MemberType::FLOAT3, minLaserEscale),
    MEMBER(MemberType::FLOAT3, maxLaserEscale),
    MEMBER(MemberType::FLOAT3, minLaserColliderSize),
    MEMBER(MemberType::FLOAT3, maxLaserColliderSize),
    MEMBER(MemberType::FLOAT3, minLaserPosition),
    MEMBER(MemberType::FLOAT3, maxLaserPosition),
    MEMBER(MemberType::FLOAT2, LaserWarningStartScale),
    MEMBER(MemberType::GAME_OBJECT_UID, laserTargetUID),
    MEMBER(MemberType::GAME_OBJECT_UID, laserWarningUID),
    MEMBER(MemberType::GAME_OBJECT_UID, pairGeneratorUID)
};

GENERATE_BODY_IMPL(MovingLasers);

void MovingLasers::Start() {
    GameObject* ownerGo = &GetOwner();
    GameObject* pair = GameplaySystems::GetGameObject(pairGeneratorUID);

    if (ownerGo) {
        animationComp = ownerGo->GetComponent<ComponentAnimation>();
        transform = ownerGo->GetComponent<ComponentTransform>();
        audioComp = ownerGo->GetComponent<ComponentAudioSource>();
    }
    if (pair) {
        pairAnimationComp = pair->GetComponent<ComponentAnimation>();
        pairScript = GET_SCRIPT(pair, MovingLasers);
    }

    laserObject = GameplaySystems::GetGameObject(laserTargetUID);
    if (laserObject) {
        laserObject->Disable();
        laserTransform = laserObject->GetComponent<ComponentTransform>();
        laserCollider = laserObject->GetComponent<ComponentBoxCollider>();
        laserObjectSFX = laserObject->GetComponent<ComponentAudioSource>();
    }

    laserWarning = GameplaySystems::GetGameObject(laserWarningUID);
    if (laserWarning) {
        laserWarning->Disable();
        laserWarningVFX = laserWarning->GetComponent<ComponentParticleSystem>();
        laserWarningSFX = laserWarning->GetComponent<ComponentAudioSource>();
    }
}

void MovingLasers::Update() {
    if (!animationComp) return;
    if (!laserTransform || !laserCollider) return;
    switch (currentState)
    {
    case GeneratorState::IDLE:
        if (beingUsed) {
                currentState = GeneratorState::START;
                if (laserWarning) {
                    laserWarning->Enable();
                    if (laserWarningVFX) {
                        laserWarningVFX->PlayChildParticles();
                        laserWarningVFX->SetLife(float2(chargingDuration, chargingDuration));
                        laserWarningVFX->SetScale(LaserWarningStartScale);

                        if (laserWarningSFX) laserWarningSFX->Play();
                    }
                }
                if (animationComp->currentStatePrincipal.name != states[static_cast<unsigned int>(GeneratorState::START)]) {
                    animationComp->SendTrigger(states[static_cast<unsigned int>(GeneratorState::IDLE)] + states[static_cast<unsigned int>(GeneratorState::START)]);
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
        if (!beingUsed) {
                currentState = GeneratorState::IDLE;
                if (!beingUsed) {
                    animationComp->SendTrigger(states[static_cast<unsigned int>(GeneratorState::SHOOT)] + states[static_cast<unsigned int>(GeneratorState::IDLE)]);
                    currentState = GeneratorState::DISABLE;
                }
        }
        else {
            Move();
        }
        break;
    case GeneratorState::DISABLE:
        movingToInit = false;
        if (Move()) {
            currentState = GeneratorState::IDLE;
            if (animationComp && animationComp->GetCurrentState()->name != states[static_cast<unsigned int>(GeneratorState::IDLE)]) {
                animationComp->SendTrigger(states[static_cast<unsigned int>(GeneratorState::SHOOT)] + states[static_cast<unsigned int>(GeneratorState::IDLE)]);
            }
        }
    }

    if (currentState != GeneratorState::SHOOT) {
        if (laserObject && laserObject->IsActive()) {
            if (laserObjectSFX) laserObjectSFX->Stop();
            laserObject->Disable();
        }
    }
    else {
        if (laserObject && !laserObject->IsActive()) {
            laserObject->Enable();
            if (laserObjectSFX) laserObjectSFX->Play();
        }
    }
}

bool MovingLasers::Move() {
    float3 destination = (movingToInit) ? initialGeneratorPosition : finalGeneratorPosition;
    float3 newLaserScale = (movingToInit) ? minLaserEscale : maxLaserEscale;
    float3 newLaserPosition = (movingToInit) ? minLaserPosition : maxLaserPosition;
    float3 newColliderSize = (movingToInit) ? minLaserColliderSize : maxLaserColliderSize;
    float3 generatorPosition = transform->GetGlobalPosition();

    generatorPosition = Lerp(generatorPosition, destination, movementSpeed * Time::GetDeltaTime());
    bool positionReached = generatorPosition.Distance(destination) < 0.5f;

    if (positionReached) {
        movingToInit = !movingToInit;
        if (pairScript && pairScript->movingToInit != movingToInit) pairScript->Synchronize(movingToInit);
        generatorPosition = destination;
        laserTransform->SetGlobalScale(newLaserScale);
        laserTransform->SetPosition(newLaserPosition);
        laserCollider->size = newColliderSize;
        Physics::UpdateRigidbody(laserCollider);
    }
    else {
        float3 laserScale = laserTransform->GetGlobalScale();
        float3 laserPosition = laserTransform->GetPosition();
        float3 laserSize = laserCollider->size;
        laserScale = Lerp(laserScale, newLaserScale, movementSpeed * Time::GetDeltaTime());
        laserPosition = Lerp(laserPosition, newLaserPosition, movementSpeed * Time::GetDeltaTime());
        laserSize = Lerp(laserSize, newColliderSize, movementSpeed * Time::GetDeltaTime());
        laserTransform->SetGlobalScale(laserScale);
        laserTransform->SetPosition(laserPosition);
        laserCollider->size = laserSize;
        Physics::UpdateRigidbody(laserCollider);
    }
    transform->SetGlobalPosition(generatorPosition);
    return positionReached;
}

void MovingLasers::TurnOn() {
    beingUsed = true;
    currentState = GeneratorState::IDLE;
    if (audioComp) audioComp->Play();
    if (pairScript && !pairScript->BeingUsed()) pairScript->TurnOn();
}

void MovingLasers::TurnOff() {
    beingUsed = false;
    currentState = GeneratorState::DISABLE;
    if (audioComp) audioComp->Stop();
    if (pairScript && pairScript->BeingUsed()) pairScript->TurnOff();
}

bool MovingLasers::BeingUsed() {
    return beingUsed;
}

void MovingLasers::Synchronize(bool movingToInit_) {
    float3 destination = (movingToInit) ? initialGeneratorPosition : finalGeneratorPosition;
    float3 newLaserScale = (movingToInit) ? minLaserEscale : maxLaserEscale;
    float3 newLaserPosition = (movingToInit) ? minLaserPosition : maxLaserPosition;
    float3 newColliderSize = (movingToInit) ? minLaserColliderSize : maxLaserColliderSize;
    laserTransform->SetGlobalScale(newLaserScale);
    laserTransform->SetPosition(newLaserPosition);
    laserCollider->size = newColliderSize;
    Physics::UpdateRigidbody(laserCollider);
    transform->SetGlobalPosition(destination);
    movingToInit = movingToInit_;
    if (pairScript->movingToInit != movingToInit) pairScript->Synchronize(movingToInit);
}