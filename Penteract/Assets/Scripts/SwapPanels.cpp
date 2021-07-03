#include "SwapPanels.h"

#include "GameplaySystems.h"

EXPOSE_MEMBERS(SwapPanels) {
    // Add members here to expose them to the engine. Example:
    // MEMBER(MemberType::BOOL, exampleMember1),
    // MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
    MEMBER(MemberType::GAME_OBJECT_UID, targetUID),
        MEMBER(MemberType::GAME_OBJECT_UID, currentUID)
};

GENERATE_BODY_IMPL(SwapPanels);

void SwapPanels::Start() {
    
    target = GameplaySystems::GetGameObject(targetUID);
    current = GameplaySystems::GetGameObject(currentUID);

    selectable = GetOwner().GetComponent < ComponentSelectable>();

    int i = 0;
    for (ComponentAudioSource& src : GetOwner().GetComponents<ComponentAudioSource>()) {
        if (i < static_cast<int>(AudioType::TOTAL)) audios[i] = &src;
        ++i;
    }
}

void SwapPanels::Update() {
    if (selectable) {
        ComponentSelectable* hoveredComponent = UserInterface::GetCurrentEventSystem()->GetCurrentlyHovered();
        if (hoveredComponent) {
            bool hovered = selectable->GetID() == hoveredComponent->GetID() ? true : false;
            if (hovered) {
                if (playHoveredAudio) {
                    PlayAudio(AudioType::HOVERED);
                    playHoveredAudio = false;
                }
            }
            else {
                playHoveredAudio = true;
            }
        }
        else {
            playHoveredAudio = true;
        }
    }
}

void SwapPanels::OnButtonClick()
{
    PlayAudio(AudioType::CLICKED);

    if (target != nullptr && current != nullptr) {
        target->Enable();
        current->Disable();
    }
}

void SwapPanels::PlayAudio(AudioType type) {
    if (audios[static_cast<int>(type)]) audios[static_cast<int>(type)]->Play();
}
