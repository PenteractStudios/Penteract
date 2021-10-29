#include "SFXGainController.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "Components/UI/ComponentSlider.h"

EXPOSE_MEMBERS(SFXGainController) {
    // Add members here to expose them to the engine. Example:
    // MEMBER(MemberType::BOOL, exampleMember1),
    // MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
    // MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
};

GENERATE_BODY_IMPL(SFXGainController);

void SFXGainController::Start() {
    slider = GetOwner().GetComponent<ComponentSlider>();
}

void SFXGainController::Update() {
    if (!slider || isUpdated) return;

    slider->ChangeNormalizedValue(Audio::GetGainSFXChannel());
    isUpdated = true;
}

void SFXGainController::OnValueChanged() {
    if (!slider) return;

    float volume = slider->GetNormalizedValue();
    Audio::SetGainSFXChannel(volume);
}