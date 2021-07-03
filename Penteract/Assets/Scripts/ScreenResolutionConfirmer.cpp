#include "ScreenResolutionConfirmer.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(ScreenResolutionConfirmer) {

};

GENERATE_BODY_IMPL(ScreenResolutionConfirmer);

void ScreenResolutionConfirmer::Start() {
    int i = 0;
    for (ComponentAudioSource& src : GetOwner().GetComponents<ComponentAudioSource>()) {
        if (i < static_cast<int>(AudioType::TOTAL)) audios[i] = &src;
        ++i;
    }
}

void ScreenResolutionConfirmer::Update() {
    PlayAudio(AudioType::CLICKED);
    screenResolutionChangeConfirmationWasRequested = true;
}

void ScreenResolutionConfirmer::OnButtonClick() {
	screenResolutionChangeConfirmationWasRequested = true;
}

void ScreenResolutionConfirmer::PlayAudio(AudioType type) {
    if (audios[static_cast<int>(type)]) audios[static_cast<int>(type)]->Play();
}
