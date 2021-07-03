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
	int i = 0;
	for (ComponentAudioSource& src : GetOwner().GetComponents<ComponentAudioSource>()) {
		if (i < static_cast<int>(AudioType::TOTAL)) audios[i] = &src;
		++i;
	}
}

void ExitButton::Update() {
}

void ExitButton::OnButtonClick() {
	PlayAudio(AudioType::CLICKED);
	SceneManager::ExitGame();
}

void ExitButton::PlayAudio(AudioType type) {
	if (audios[static_cast<int>(type)]) audios[static_cast<int>(type)]->Play();
}