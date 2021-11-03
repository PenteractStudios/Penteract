#include "SwapPanels.h"

#include "GameplaySystems.h"

EXPOSE_MEMBERS(SwapPanels) {
	MEMBER(MemberType::GAME_OBJECT_UID, targetUID),
	MEMBER(MemberType::GAME_OBJECT_UID, currentUID),
	MEMBER(MemberType::GAME_OBJECT_UID, optionalVideoUID)
};

GENERATE_BODY_IMPL(SwapPanels);

void SwapPanels::Start() {

	target = GameplaySystems::GetGameObject(targetUID);
	current = GameplaySystems::GetGameObject(currentUID);

	/* Video */
	GameObject* videoObj = GameplaySystems::GetGameObject(optionalVideoUID);
	if (videoObj) {
		video = videoObj->GetComponent<ComponentVideo>();
		if (video) video->Stop();
	}
	/* Audio */
	selectable = GetOwner().GetComponent<ComponentSelectable>();
}

void SwapPanels::Update() {
}

void SwapPanels::OnButtonClick() {
	DoSwapPanels();
}

void SwapPanels::DoSwapPanels() {
	if (target != nullptr && current != nullptr) {
		current->Disable();
		target->Enable();

		// TODO: This is a very ugly solution. An object OnEnable() function, or a video IsPlaying(), would be needed to have this in a separate script
		if (video) {
			if (current->name == "CanvasCredits") video->Stop();
			if (target->name == "CanvasCredits") video->Play();
		}
	}
}
