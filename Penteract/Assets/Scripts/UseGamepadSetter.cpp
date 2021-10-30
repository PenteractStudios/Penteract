#include "UseGamepadSetter.h"

#include "PlayerController.h"
#include "GlobalVariables.h"

EXPOSE_MEMBERS(UseGamepadSetter) {
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

	GameplaySystems::SetGlobalVariable<bool>(globalUseGamepad, toggled);
}

void UseGamepadSetter::SearchForPlayerRef() {
	if (playerController) return;

	GameObject* playerObj = GameplaySystems::GetGameObject("Player");

	if (playerObj) {
		playerController = GET_SCRIPT(playerObj, PlayerController);
	}

}
