#include "MovingLasers.h"

#include "GameplaySystems.h"
#include "GameObject.h"

EXPOSE_MEMBERS(MovingLasers) {
    MEMBER_SEPARATOR("Timers"),
    MEMBER(MemberType::FLOAT, chargingDuration),
    MEMBER(MemberType::FLOAT, waitingDuration),
    MEMBER(MemberType::FLOAT, movementSpeed),
    MEMBER_SEPARATOR("Transforms"),
    MEMBER(MemberType::FLOAT3, initialGeneratorPosition),
    MEMBER(MemberType::FLOAT3, finalGeneratorPosition),
    MEMBER(MemberType::FLOAT3, minLaserEscale),
    MEMBER(MemberType::FLOAT3, maxLaserEscale),
    MEMBER(MemberType::FLOAT3, minLaserColliderSize),
    MEMBER(MemberType::FLOAT3, maxLaserColliderSize),
    MEMBER(MemberType::FLOAT3, minLaserPosition),
    MEMBER(MemberType::FLOAT3, maxLaserPosition),
    MEMBER(MemberType::FLOAT2, LaserWarningStartScale),
    MEMBER_SEPARATOR("UIDs"),
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
        if (transform) {
            generatorPosition = transform->GetGlobalPosition();
        }
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
        if (laserTransform) {
            laserScale = laserTransform->GetGlobalScale();
            laserPosition = laserTransform->GetPosition();
        }
        if (laserCollider = laserObject->GetComponent<ComponentBoxCollider>()) {
            laserColliderSize = laserCollider->size;
        }
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
                    totalDistance = transform->GetGlobalPosition().Distance((movingToInit) ? initialGeneratorPosition : finalGeneratorPosition);
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
            break;
        case GeneratorState::WAIT:
            if (waitingTimer <= waitingDuration) {
                waitingTimer += Time::GetDeltaTime();
                if (waitingTimer > waitingDuration) {
                    waitingTimer = 0.f;
                    currentState = GeneratorState::SHOOT;
                }
            }
    }

    if (currentState != GeneratorState::SHOOT && currentState != GeneratorState::WAIT) {
        if (laserObject && laserObject->IsActive()) laserObject->Disable();
        if (laserCollider && laserCollider->IsActive()) laserCollider->Disable();
    }
    else {
        if (laserObject && !laserObject->IsActive())  laserObject->Enable();
        if (laserCollider && !laserCollider->IsActive()) laserCollider->Enable();
    }
}

bool MovingLasers::Move() {
    float3 destination = (movingToInit) ? initialGeneratorPosition : finalGeneratorPosition;
    float3 destLaserScale = (movingToInit) ? minLaserEscale : maxLaserEscale;
    float3 destLaserPosition = (movingToInit) ? minLaserPosition : maxLaserPosition;
    float3 destColliderSize = (movingToInit) ? minLaserColliderSize : maxLaserColliderSize;
    float distance = transform->GetGlobalPosition().Distance(destination);
    float speed = movementSpeed * Time::GetDeltaTime() + ((totalDistance - distance) / totalDistance);
    bool positionReached = distance < 0.2f;

    if (positionReached) {
        if (currentState != GeneratorState::DISABLE) currentState = GeneratorState::WAIT;

        movingToInit = !movingToInit;
        if (pairScript && pairScript->movingToInit != movingToInit) pairScript->Synchronize(movingToInit);

        transform->SetGlobalPosition(destination);
        generatorPosition = destination;

        laserTransform->SetGlobalScale(destLaserScale);
        laserScale = destLaserScale;

        laserTransform->SetPosition(destLaserPosition);
        laserPosition = destLaserPosition;

        laserCollider->size = destColliderSize;
        laserColliderSize = destColliderSize;
    }
    else {
        transform->SetGlobalPosition(Lerp(generatorPosition, destination, speed));
        laserTransform->SetGlobalScale(Lerp(laserScale, destLaserScale, speed));
        laserTransform->SetPosition(Lerp(laserPosition, destLaserPosition, speed));
        laserCollider->size = Lerp(laserColliderSize, destColliderSize, speed);
    }
  
    Physics::UpdateRigidbody(laserCollider);
    if (currentState != GeneratorState::SHOOT && currentState != GeneratorState::WAIT) laserCollider->Disable();

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

    transform->SetGlobalPosition(destination);
    generatorPosition = destination;

    laserTransform->SetGlobalScale(newLaserScale);
    laserScale = newLaserScale;

    laserTransform->SetPosition(newLaserPosition);
    laserPosition = newLaserPosition;

    laserCollider->size = newColliderSize;
    laserColliderSize = newColliderSize;

    Physics::UpdateRigidbody(laserCollider);
    if (currentState != GeneratorState::DISABLE) currentState = GeneratorState::WAIT;
    if (currentState != GeneratorState::SHOOT && currentState != GeneratorState::WAIT) laserCollider->Disable();

    movingToInit = movingToInit_;
    
    if (pairScript->movingToInit != movingToInit) pairScript->Synchronize(movingToInit);
}