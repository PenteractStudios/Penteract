#include "ReturnMenu.h"

#include "GameObject.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(ReturnMenu) {
	MEMBER(MemberType::SCENE_RESOURCE_UID, sceneUID),
	MEMBER(MemberType::FLOAT, padding)
};

GENERATE_BODY_IMPL(ReturnMenu);

void ReturnMenu::Start() {
    /* Audio */
    selectable = GetOwner().GetComponent<ComponentSelectable>();

    int i = 0;
    for (ComponentAudioSource& src : GetOwner().GetComponents<ComponentAudioSource>()) {
        if (i < static_cast<int>(AudioType::TOTAL)) audios[i] = &src;
        ++i;
    }
}

void ReturnMenu::Update() {
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

void ReturnMenu::OnButtonClick() {
    PlayAudio(AudioType::CLICKED);
	if(sceneUID != 0)SceneManager::ChangeScene(sceneUID);
	if (Time::GetDeltaTime() == 0.f) Time::ResumeGame();
}

void ReturnMenu::PlayAudio(AudioType type) {
    if (audios[static_cast<int>(type)]) audios[static_cast<int>(type)]->Play();
}
