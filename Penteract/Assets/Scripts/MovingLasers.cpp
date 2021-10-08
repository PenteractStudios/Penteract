#include "MovingLasers.h"

#include "GameplaySystems.h"
#include "GameObject.h"

EXPOSE_MEMBERS(MovingLasers) {
    MEMBER(MemberType::FLOAT, chargingDuration),
    MEMBER(MemberType::FLOAT3, InitialPosition),
    MEMBER(MemberType::FLOAT3, FinalPosition),
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
    }

    TurnOn(); //remove before pr
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
                    ComponentParticleSystem* laserWarningVFX = laserWarning->GetComponent<ComponentParticleSystem>();
                    if (laserWarningVFX) laserWarningVFX->PlayChildParticles();
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
                }
                if (pairScript->currentState == GeneratorState::IDLE) {
                    pairAnimationComp->SendTrigger(states[static_cast<unsigned int>(GeneratorState::SHOOT)] + states[static_cast<unsigned int>(GeneratorState::IDLE)]);
                }
        }
        else {
            //moving code
            float3 destination = (movingToInit) ? InitialPosition : FinalPosition;
            float3 position = transform->GetGlobalPosition();
            position = float3::Lerp(position, destination, movementSpeed * Time::GetDeltaTime());
            
            transform->SetGlobalPosition(position);
            if (position.Distance(destination) < 0.1f) {
                movingToInit = !movingToInit;
            }
            else {
                float scale = (movingToInit) ? movementSpeed : -movementSpeed;
                float newScale = laserTransform->GetGlobalScale().y - (scale * Time::GetDeltaTime());
                float3 newPosition = float3(laserTransform->GetPosition().x - (scale * 9.8f * Time::GetDeltaTime()), laserTransform->GetPosition().y, laserTransform->GetPosition().z);
                if (!(newScale > maxLaserEscale || newScale < minLaserEscale)) {
                    laserTransform->SetGlobalScale(float3(laserTransform->GetGlobalScale().x, newScale, laserTransform->GetGlobalScale().z));
                    laserTransform->SetPosition(newPosition);
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