#include "MovingLasers.h"

#include "GameplaySystems.h"
#include "GameObject.h"

EXPOSE_MEMBERS(MovingLasers) {
    MEMBER(MemberType::FLOAT, chargingDuration),
    MEMBER(MemberType::FLOAT3, initialGeneratorPosition),
    MEMBER(MemberType::FLOAT3, finalGeneratorPosition),
    MEMBER(MemberType::FLOAT3, minLaserEscale),
    MEMBER(MemberType::FLOAT3, maxLaserEscale),
    MEMBER(MemberType::FLOAT3, minLaserPosition),
    MEMBER(MemberType::FLOAT3, maxLaserPosition),
    MEMBER(MemberType::FLOAT2, minLaserWarningScale),
    MEMBER(MemberType::FLOAT2, maxLaserWarningScale),
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
    }
    if (pair) {
        pairAnimationComp = pair->GetComponent<ComponentAnimation>();
        pairScript = GET_SCRIPT(pair, MovingLasers);
    }

    laserObject = GameplaySystems::GetGameObject(laserTargetUID);
    if (laserObject) {
        laserObject->Disable();
        laserTransform = laserObject->GetComponent<ComponentTransform>();
    }

    laserWarning = GameplaySystems::GetGameObject(laserWarningUID);
    if (laserWarning) {
        laserWarning->Disable();
        laserWarningVFX = laserWarning->GetComponent<ComponentParticleSystem>();
    }
}

void MovingLasers::Update() {
    if (!animationComp) return;

    switch (currentState)
    {
    case GeneratorState::IDLE:
        if (beingUsed) {
                currentState = GeneratorState::START;
                if (laserWarning) {
                    laserWarning->Enable();
                    if (laserWarningVFX) laserWarningVFX->PlayChildParticles();
                    laserWarningVFX->SetLife(float2(chargingDuration, chargingDuration));
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
            //Move();
        }
        break;

    case GeneratorState::SHOOT:
        if (!beingUsed) {
                currentState = GeneratorState::IDLE;
                if (!beingUsed) {
                    animationComp->SendTrigger(states[static_cast<unsigned int>(GeneratorState::SHOOT)] + states[static_cast<unsigned int>(GeneratorState::IDLE)]);
                }
                if (pairScript->currentState == GeneratorState::IDLE) {
                    pairAnimationComp->SendTrigger(states[static_cast<unsigned int>(GeneratorState::SHOOT)] + states[static_cast<unsigned int>(GeneratorState::IDLE)]);
                }
        }
        else {
            Move();
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

void MovingLasers::Move() {

    float3 destination = (movingToInit) ? initialGeneratorPosition : finalGeneratorPosition;
    float3 newLaserScale = (movingToInit) ? minLaserEscale : maxLaserEscale;
    float3 newLaserPosition = (movingToInit) ? minLaserPosition : maxLaserPosition;
    float2 newLaserWarningScale = (movingToInit) ? minLaserWarningScale : maxLaserWarningScale;
    float3 generatorPosition = transform->GetGlobalPosition();

    generatorPosition = Lerp(generatorPosition, destination, movementSpeed * Time::GetDeltaTime());

    if (generatorPosition.Distance(destination) < 0.5f) {
        movingToInit = !movingToInit;
        if (pairScript->movingToInit != movingToInit) pairScript->Synchronize(movingToInit);
        generatorPosition = destination;
        laserTransform->SetGlobalScale(newLaserScale);
        laserTransform->SetPosition(newLaserPosition);
        laserWarningVFX->SetScale(newLaserWarningScale);
    }
    else {
        float3 laserScale = laserTransform->GetGlobalScale();
        float3 laserPosition = laserTransform->GetPosition();
        float2 laserWarningScale = laserWarningVFX->GetScale();
        laserScale = Lerp(laserScale, newLaserScale, movementSpeed * Time::GetDeltaTime());
        laserPosition = Lerp(laserPosition, newLaserPosition, movementSpeed * Time::GetDeltaTime());
        laserWarningScale = Lerp(laserWarningScale, newLaserWarningScale, movementSpeed * Time::GetDeltaTime());
        laserTransform->SetGlobalScale(laserScale);
        laserTransform->SetPosition(laserPosition);
        laserWarningVFX->SetScale(laserWarningScale);
    }
    transform->SetGlobalPosition(generatorPosition);
}

void MovingLasers::TurnOn() {
    beingUsed = true;
    currentState = GeneratorState::IDLE;
    if (pairScript && !pairScript->BeingUsed()) pairScript->TurnOn();
}

void MovingLasers::TurnOff() {
    beingUsed = false;
    currentState = GeneratorState::IDLE;
    if (pairScript && !pairScript->BeingUsed()) pairScript->TurnOff();
}

bool MovingLasers::BeingUsed() {
    return beingUsed;
}

void MovingLasers::Synchronize(bool movingToInit_) {
    float3 destination = (movingToInit) ? initialGeneratorPosition : finalGeneratorPosition;
    float3 newLaserScale = (movingToInit) ? minLaserEscale : maxLaserEscale;
    float3 newLaserPosition = (movingToInit) ? minLaserPosition : maxLaserPosition;
    laserTransform->SetGlobalScale(newLaserScale);
    laserTransform->SetPosition(newLaserPosition);
    transform->SetGlobalPosition(destination);
    movingToInit = movingToInit_;
    if (pairScript->movingToInit != movingToInit) pairScript->Synchronize(movingToInit);
}