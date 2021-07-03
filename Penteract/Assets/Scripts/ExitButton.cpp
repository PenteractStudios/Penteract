#include "ExitButton.h"

#include "GameplaySystems.h"

EXPOSE_MEMBERS(ExitButton) {
	// Add members here to expose them to the engine. Example:
	// MEMBER(MemberType::BOOL, exampleMember1),
	// MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
	// MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
};

GENERATE_BODY_IMPL(ExitButton);

void ExitButton::Start() {

    /* Audio */
	selectable = GetOwner().GetComponent<ComponentSelectable>();

	int i = 0;
	for (ComponentAudioSource& src : GetOwner().GetComponents<ComponentAudioSource>()) {
		if (i < static_cast<int>(AudioType::TOTAL)) audios[i] = &src;
		++i;
	}
}

void ExitButton::Update() {
    /* Audio */
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

void ExitButton::OnButtonClick() {
	PlayAudio(AudioType::CLICKED);
	SceneManager::ExitGame();
}

void ExitButton::PlayAudio(AudioType type) {
	if (audios[static_cast<int>(type)]) audios[static_cast<int>(type)]->Play();
}