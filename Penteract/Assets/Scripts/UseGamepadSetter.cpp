#include "UseGamepadSetter.h"
#include "PlayerController.h"

EXPOSE_MEMBERS(UseGamepadSetter) {
	// Add members here to expose them to the engine. Example:
	// MEMBER(MemberType::BOOL, exampleMember1),
	// MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
	// MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
};

GENERATE_BODY_IMPL(UseGamepadSetter);

void UseGamepadSetter::Start() {
	SearchForPlayerRef();
}

void UseGamepadSetter::Update() {

}

void UseGamepadSetter::OnToggled(bool toggled) {

	SearchForPlayerRef();

	if (!playerController) return;

	playerController->SetUseGamepad(toggled);

}

void UseGamepadSetter::SearchForPlayerRef() {
	if (playerController) return;

	GameObject* playerObj = GameplaySystems::GetGameObject("Player");

	if (playerObj) {
		playerController = GET_SCRIPT(playerObj, PlayerController);
	}

}
