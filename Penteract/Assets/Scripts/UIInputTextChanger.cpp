#include "UIInputTextChanger.h"

#include "GameplaySystems.h"
#include "GlobalVariables.h"
#include "Components/UI/ComponentText.h"

EXPOSE_MEMBERS(UIInputTextChanger) {
	MEMBER(MemberType::STRING, keyboardText),
	MEMBER(MemberType::STRING, gameControllerText)
		
};

GENERATE_BODY_IMPL(UIInputTextChanger);



void UIInputTextChanger::Start() {

	textToModify = GetOwner().GetComponent<ComponentText>();

	SetTextToKeyboard();

}

void UIInputTextChanger::Update() {

	bool gamepad = GameplaySystems::GetGlobalVariable(globalUseGamepad, true) && Input::IsGamepadConnected(0);

	if (lastSerializedGamepadOn) {
		if (!gamepad) {
			SetTextToKeyboard();
		}
	} else {
		if (gamepad) {
			SetTextToGamepad();
		}
	}
	lastSerializedGamepadOn = gamepad;

}

void UIInputTextChanger::SetTextToKeyboard() {
	if (!textToModify) return;
	textToModify->SetText(keyboardText);
}

void UIInputTextChanger::SetTextToGamepad() {
	if (!textToModify) return;
	textToModify->SetText(gameControllerText);
}
