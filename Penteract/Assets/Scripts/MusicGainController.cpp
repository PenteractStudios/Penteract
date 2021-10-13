#include "MusicGainController.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "Components/UI/ComponentSlider.h"

EXPOSE_MEMBERS(MusicGainController) {
    // Add members here to expose them to the engine. Example:
    // MEMBER(MemberType::BOOL, exampleMember1),
    // MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
    // MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
};

GENERATE_BODY_IMPL(MusicGainController);

void MusicGainController::Start() {
    slider = GetOwner().GetComponent<ComponentSlider>();
}

void MusicGainController::Update() {
    if (!slider || isUpdated) return;

    slider->ChangeNormalizedValue(Audio::GetGainMusicChannel());
    isUpdated = true;
}

void MusicGainController::OnValueChanged() {
    if (!slider) return;

    float volume = slider->GetNormalizedValue();
    Audio::SetGainMusicChannel(volume);
}