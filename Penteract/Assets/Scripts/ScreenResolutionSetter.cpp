#include "ScreenResolutionSetter.h"

#include "GameplaySystems.h"
#include "Components/UI/ComponentText.h"
#include "Modules/ModuleWindow.h"
#include "GameObject.h"
#include "ScreenResolutionConfirmer.h"

bool screenResolutionChangeConfirmationWasRequested;
unsigned preSelectedScreenResolutionPreset;

EXPOSE_MEMBERS(ScreenResolutionSetter) {

	MEMBER(MemberType::GAME_OBJECT_UID, textObjectID),
		MEMBER(MemberType::BOOL, increasing)
};

GENERATE_BODY_IMPL(ScreenResolutionSetter);

void ScreenResolutionSetter::Start() {
	GameObject* textObj = GameplaySystems::GetGameObject(textObjectID);

	if (textObj) {
		text = textObj->GetComponent<ComponentText>();
	}

	screenResolutionChangeConfirmationWasRequested = false;
	preSelectedScreenResolutionPreset = Screen::GetCurrentDisplayMode();
	UpdateResolution();
}

void ScreenResolutionSetter::Update() {
	if (screenResolutionChangeConfirmationWasRequested) {
		if (Screen::GetCurrentDisplayMode() != preSelectedScreenResolutionPreset) {
			Screen::SetCurrentDisplayMode(preSelectedScreenResolutionPreset);
		}
		screenResolutionChangeConfirmationWasRequested = false;
	}
}

void ScreenResolutionSetter::OnButtonClick() {
	IncreaseResolution(increasing ? -1 : 1);		// This is reversed because the list obtained goes from highest to lowest, being position 0 the highest.
}

void ScreenResolutionSetter::IncreaseResolution(int multiplier) {
	int newSelectedPreset = preSelectedScreenResolutionPreset + multiplier;

	//Avoid getting out of bounds
	if (newSelectedPreset >= 0 && newSelectedPreset < Screen::GetNumDisplayModes()) {
		preSelectedScreenResolutionPreset = newSelectedPreset;
		UpdateResolution();
	}
}

void ScreenResolutionSetter::UpdateResolution() {
	if (!text) return;

	Screen::DisplayMode displayMode = Screen::GetDisplayMode(preSelectedScreenResolutionPreset);
	text->SetText(std::to_string(displayMode.width) + " X " + std::to_string(displayMode.height) + " (" + std::to_string(displayMode.hz) + "hz)");
}