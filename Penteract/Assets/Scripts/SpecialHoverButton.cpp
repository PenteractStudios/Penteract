#include "SpecialHoverButton.h"

#include "GameplaySystems.h"
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentSelectable.h"

EXPOSE_MEMBERS(SpecialHoverButton) {
	MEMBER(MemberType::GAME_OBJECT_UID, buttonIdleImageObjUID),
	MEMBER(MemberType::GAME_OBJECT_UID, buttonHoveredImageObjUID),
	MEMBER(MemberType::GAME_OBJECT_UID, buttonClickedImageObjUID),
};

GENERATE_BODY_IMPL(SpecialHoverButton);

void SpecialHoverButton::Start() {
	GameObject* buttonIdleImageObj = GameplaySystems::GetGameObject(buttonIdleImageObjUID);
	GameObject* buttonHoveredImageObj = GameplaySystems::GetGameObject(buttonHoveredImageObjUID);
	GameObject* buttonClickedImageObj = GameplaySystems::GetGameObject(buttonClickedImageObjUID);

	if (buttonIdleImageObj)	   buttonIdleImage = buttonIdleImageObj->GetComponent<ComponentImage>();
	if (buttonHoveredImageObj) buttonHoveredImage = buttonHoveredImageObj->GetComponent<ComponentImage>();
	if (buttonClickedImageObj) buttonClickedImage = buttonClickedImageObj->GetComponent<ComponentImage>();

	if (buttonIdleImage)buttonIdleImage->Enable();
	if (buttonHoveredImage)buttonHoveredImage->Disable();
	if (buttonClickedImage)buttonClickedImage->Disable();


	selectable = GetOwner().GetComponent<ComponentSelectable>();

}

void SpecialHoverButton::Update() {
	if (!buttonIdleImage || !buttonHoveredImage || !buttonClickedImage)return;
	switch (buttonState) {
	case ButtonState::IDLE:
		if (selectable) {
			ComponentEventSystem* eventSystem = UserInterface::GetCurrentEventSystem();
			if (eventSystem) {
				ComponentSelectable* hoveredComponent = eventSystem->GetCurrentlyHovered();
				if (hoveredComponent) {
					bool hovered = selectable->GetID() == hoveredComponent->GetID() ? true : false;

					if (hovered) {
						EnterButtonState(ButtonState::HOVERED);
					}
				}
			}
		}
		break;
	case ButtonState::HOVERED:
		if (selectable) {
			ComponentEventSystem* eventSystem = UserInterface::GetCurrentEventSystem();
			if (eventSystem) {
				ComponentSelectable* hoveredComponent = eventSystem->GetCurrentlyHovered();
				if (hoveredComponent) {
					bool hovered = selectable->GetID() == hoveredComponent->GetID() ? true : false;

					if (!hovered) {
						EnterButtonState(ButtonState::IDLE);
					}

				} else {
					EnterButtonState(ButtonState::IDLE);
				}
			}

			if (Input::GetMouseButton(0)) {
				EnterButtonState(ButtonState::CLICKED);
			}
		}
		break;
	case ButtonState::CLICKED:
		if (!Input::GetMouseButton(0)) {
			EnterButtonState(ButtonState::IDLE);
		}
	}
}

void SpecialHoverButton::OnButtonClick() {
	EnterButtonState(ButtonState::IDLE);
}

void SpecialHoverButton::EnterButtonState(ButtonState newState) {
	if (!buttonIdleImage || !buttonHoveredImage || !buttonClickedImage)return;
	switch (newState) {
	case ButtonState::IDLE:
		buttonHoveredImage->Disable();
		buttonClickedImage->Disable();
		buttonIdleImage->Enable();
		break;
	case ButtonState::HOVERED:
		buttonHoveredImage->Enable();
		buttonClickedImage->Disable();
		buttonIdleImage->Disable();
		break;
	case ButtonState::CLICKED:
		buttonHoveredImage->Disable();
		buttonClickedImage->Enable();
		buttonIdleImage->Disable();
		break;
	}
	buttonState = newState;
}
