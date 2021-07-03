#include "StartButton.h"

#include "CheckpointManager.h"

#include "GameplaySystems.h"
#include "GameObject.h"

int checkpoint;

EXPOSE_MEMBERS(StartButton) {
	MEMBER(MemberType::SCENE_RESOURCE_UID, sceneUID),
	MEMBER(MemberType::INT, checkpointNum)
};


GENERATE_BODY_IMPL(StartButton);

void StartButton::Start() {
    /* Audio */
	selectable = GetOwner().GetComponent<ComponentSelectable>();

	int i = 0;
	for (ComponentAudioSource& src : GetOwner().GetComponents<ComponentAudioSource>()) {
		if (i < static_cast<int>(AudioType::TOTAL)) audios[i] = &src;
		++i;
	}
}

void StartButton::Update() {
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

void StartButton::OnButtonClick() {

    PlayAudio(AudioType::CLICKED);

	checkpoint = checkpointNum;
	if(sceneUID != 0) SceneManager::ChangeScene(sceneUID);
	if (Time::GetDeltaTime() == 0.f) Time::ResumeGame();
}

void StartButton::PlayAudio(AudioType type) {
    if (audios[static_cast<int>(type)]) audios[static_cast<int>(type)]->Play();
}