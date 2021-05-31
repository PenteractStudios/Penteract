#include "LowHPWarning.h"
#include "GameObject.h"
#include "Components/UI/ComponentImage.h"

#include "GameplaySystems.h"

EXPOSE_MEMBERS(LowHPWarning) {
    MEMBER(MemberType::FLOAT, effectTime),
    MEMBER(MemberType::GAME_OBJECT_UID, effectUID)
};

GENERATE_BODY_IMPL(LowHPWarning);

void LowHPWarning::Start() {
    effect = GameplaySystems::GetGameObject(effectUID);
    if (effect) {
        vignette = effect->GetComponent<ComponentImage>();
        effect->Disable();
    }
    Debug::Log("Script initialization");
}

void LowHPWarning::Update() {
    if (!effect || !vignette) return;
    if (playing) {
        fadeOutTime = effectTime / 2.0f;
        bool doFadeOut = effectCurrentTime >= fadeOutTime ? true : false;
        if (effectCurrentTime >= effectTime) {
            vignette->SetColor(float4(190.f/255.f, 0, 67.f/255.0f, 0.5f));
            effectCurrentTime = 0.0f;
            fadeOutCurrentTime = 0.0f;
        }
        else {
            if (doFadeOut) {
                if (fadeOutCurrentTime <= fadeOutTime) {
                    float delta = fadeOutCurrentTime / fadeOutTime;
                    float fadeValue = Lerp(0.5, 0, delta);
                    vignette->SetColor(float4(190.f / 255.f, 0, 67.f / 255.0f, fadeValue));
                    fadeOutCurrentTime += Time::GetDeltaTime();
                }
            }
        }
        effectCurrentTime += Time::GetDeltaTime(); 
    }
}

void LowHPWarning::Play() {
    if (!effect || !vignette) return;
    playing = true;
    effect->Enable();
}

void LowHPWarning::Stop() {
    if (!effect || !vignette) return;
    playing = false;
    effectCurrentTime = 0.0f;
    fadeOutCurrentTime = 0.0f;
    effect->Disable();
}
