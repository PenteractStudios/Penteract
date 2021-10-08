#include "SwapPanels.h"

#include "GameplaySystems.h"

EXPOSE_MEMBERS(SwapPanels) {
	MEMBER(MemberType::GAME_OBJECT_UID, targetUID),
	MEMBER(MemberType::GAME_OBJECT_UID, currentUID)
};

GENERATE_BODY_IMPL(SwapPanels);

void SwapPanels::Start() {

	target = GameplaySystems::GetGameObject(targetUID);
	current = GameplaySystems::GetGameObject(currentUID);

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
		target->Enable();
		current->Disable();
	}
}
