#include "MainGainController.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "Components/UI/ComponentSlider.h"

EXPOSE_MEMBERS(MainGainController) {
    // Add members here to expose them to the engine. Example:
    // MEMBER(MemberType::BOOL, exampleMember1),
    // MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
    // MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
};

GENERATE_BODY_IMPL(MainGainController);

void MainGainController::Start() {
    slider = GetOwner().GetComponent<ComponentSlider>();
}

void MainGainController::Update() {
    if (!slider || isUpdated) return;

    slider->ChangeNormalizedValue(Audio::GetGainMainChannel());
    isUpdated = true;
	
}

void MainGainController::OnValueChanged() {
    if (!slider) return;

    float volume = slider->GetNormalizedValue();
    Audio::SetGainMainChannel(volume);
}