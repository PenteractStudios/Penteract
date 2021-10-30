#include "UIInputTextureChanger.h"

#include "GlobalVariables.h"
#include "GameplaySystems.h"
#include "Components/UI/ComponentImage.h"

EXPOSE_MEMBERS(UIInputTextureChanger) {
	MEMBER(MemberType::GAME_OBJECT_UID, keyboardKeyImageObjUID),
	MEMBER(MemberType::GAME_OBJECT_UID, gameControllerKeyImageObjUID)
};

GENERATE_BODY_IMPL(UIInputTextureChanger);

void UIInputTextureChanger::Start() {
	if (gameControllerKeyImageObjUID != 0) {
		gameControllerKeyImageObj = GameplaySystems::GetGameObject(gameControllerKeyImageObjUID);
	}

	if (keyboardKeyImageObjUID != 0) {
		keyboardKeyImageObj = GameplaySystems::GetGameObject(keyboardKeyImageObjUID);
	}

	EnableKeyboardObj();

}

void UIInputTextureChanger::Update() {

	bool gamepad = GameplaySystems::GetGlobalVariable(globalUseGamepad, false) && Input::IsGamepadConnected(0);

	if (lastSerializedGamepadOn) {
		if (!gamepad) {
			EnableKeyboardObj();
		}
	} else {
		if (gamepad) {
			EnableGamepadObj();
		}
	}
	lastSerializedGamepadOn = gamepad;

}

ComponentImage* UIInputTextureChanger::GetActiveImage() {
	if (!gameControllerKeyImageObj || !keyboardKeyImageObj) return nullptr;

	return lastSerializedGamepadOn ? gameControllerKeyImageObj->GetComponent<ComponentImage>() : keyboardKeyImageObj->GetComponent<ComponentImage>();
}

void UIInputTextureChanger::EnableGamepadObj() {
	if (!gameControllerKeyImageObj || !keyboardKeyImageObj) return;
	gameControllerKeyImageObj->Enable();
	keyboardKeyImageObj->Disable();
}

void UIInputTextureChanger::EnableKeyboardObj() {
	if (!gameControllerKeyImageObj || !keyboardKeyImageObj) return;
	gameControllerKeyImageObj->Disable();
	keyboardKeyImageObj->Enable();
}
