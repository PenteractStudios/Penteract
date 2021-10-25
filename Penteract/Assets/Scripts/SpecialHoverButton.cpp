#include "SpecialHoverButton.h"

#include "GameplaySystems.h"
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentSelectable.h"
#include "Components/UI/ComponentText.h"
#include "Components/ComponentAudioSource.h"

EXPOSE_MEMBERS(SpecialHoverButton) {
	MEMBER(MemberType::GAME_OBJECT_UID, buttonIdleImageObjUID),
		MEMBER(MemberType::GAME_OBJECT_UID, buttonHoveredImageObjUID),
		MEMBER(MemberType::GAME_OBJECT_UID, buttonClickedImageObjUID),
		MEMBER(MemberType::GAME_OBJECT_UID, buttonTextWhiteObjUID),
		MEMBER(MemberType::GAME_OBJECT_UID, buttonTextShadowObjUID),
		MEMBER(MemberType::GAME_OBJECT_UID, audioSourcesUID)
};

GENERATE_BODY_IMPL(SpecialHoverButton);

void SpecialHoverButton::Start() {
	GameObject* buttonIdleImageObj = GameplaySystems::GetGameObject(buttonIdleImageObjUID);
	GameObject* buttonHoveredImageObj = GameplaySystems::GetGameObject(buttonHoveredImageObjUID);
	GameObject* buttonClickedImageObj = GameplaySystems::GetGameObject(buttonClickedImageObjUID);
	GameObject* buttonTextObj = GameplaySystems::GetGameObject(buttonTextWhiteObjUID);
	GameObject* buttonTextShadowObj = GameplaySystems::GetGameObject(buttonTextShadowObjUID);

	if (buttonIdleImageObj)	   buttonIdleImage = buttonIdleImageObj->GetComponent<ComponentImage>();
	if (buttonHoveredImageObj) buttonHoveredImage = buttonHoveredImageObj->GetComponent<ComponentImage>();
	if (buttonClickedImageObj) buttonClickedImage = buttonClickedImageObj->GetComponent<ComponentImage>();
	if (buttonTextObj) buttonText = buttonTextObj->GetComponent<ComponentText>();
	if (buttonTextObj) buttonTextShadow = buttonTextShadowObj->GetComponent<ComponentText>();

	if (buttonIdleImage) buttonIdleImage->Enable();
	if (buttonHoveredImage) buttonHoveredImage->Disable();
	if (buttonClickedImage) buttonClickedImage->Disable();
	if (buttonText) buttonText->SetFontColor(float4(1, 1, 1, 1));
	if (buttonTextShadow) buttonTextShadow->Enable();


	selectable = GetOwner().GetComponent<ComponentSelectable>();

	GameObject* audioSourcesObj = GameplaySystems::GetGameObject(audioSourcesUID);
	if (audioSourcesObj) {
		int i = 0;
		for (ComponentAudioSource& src : audioSourcesObj->GetComponents<ComponentAudioSource>()) {
			if (i < static_cast<int>(UIAudio::TOTAL)) audios[i] = &src;
			++i;
		}
	}
}

void SpecialHoverButton::Update() {
	if (!buttonIdleImage || !buttonHoveredImage || !buttonClickedImage)return;
	switch (buttonState) {
	case ButtonState::IDLE:
		if (selectable) {
			ComponentEventSystem* eventSystem = UserInterface::GetCurrentEventSystem();
			if (eventSystem) {
				ComponentSelectable* hoveredComponent = eventSystem->GetCurrentlyHovered();
				ComponentSelectable* selectedComponent = eventSystem->GetCurrentSelected();

				if (hoveredComponent) {
					bool hovered = selectable->GetID() == hoveredComponent->GetID() ? true : false;

					if (hovered) {
						EnterButtonState(ButtonState::HOVERED);
					}
				} else {
					if (selectedComponent) {
						bool selected = selectable->GetID() == selectedComponent->GetID() ? true : false;

						if (selected) {
							EnterButtonState(ButtonState::HOVERED);
						}
					}
				}
			}
		}
		break;
	case ButtonState::HOVERED:
		if (selectable) {
			ComponentEventSystem* eventSystem = UserInterface::GetCurrentEventSystem();
			if (eventSystem) {
				ComponentSelectable* selectedComponent = eventSystem->GetCurrentSelected();

				ComponentSelectable* hoveredComponent = eventSystem->GetCurrentlyHovered();
				if (hoveredComponent) {
					bool hovered = selectable->GetID() == hoveredComponent->GetID() ? true : false;

					if (!hovered) {
						EnterButtonState(ButtonState::IDLE);
					} else {
						if (Input::GetMouseButton(0)) {
							EnterButtonState(ButtonState::CLICKED);
						}
					}

					if (selectedComponent) {
						if (selectedComponent != hoveredComponent) {
							eventSystem->SetSelected(hoveredComponent->GetID());
						}
					}

				} else {
					bool selected = selectedComponent ? (selectable->GetID() == selectedComponent->GetID() ? true : false) : false;
					if (!selected) EnterButtonState(ButtonState::IDLE);
				}

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

void SpecialHoverButton::OnDisable() {
	EnterButtonState(ButtonState::IDLE);
}

void SpecialHoverButton::EnterButtonState(ButtonState newState) {
	if (!buttonIdleImage || !buttonHoveredImage || !buttonClickedImage) return;
	switch (newState) {
	case ButtonState::IDLE:
		buttonHoveredImage->Disable();
		buttonClickedImage->Disable();
		buttonIdleImage->Enable();
		if (buttonText && buttonTextShadow) {
			buttonText->SetFontColor(float4(1, 1, 1, 1));
			buttonTextShadow->Enable();
		}
		break;
	case ButtonState::HOVERED:
		PlayAudio(UIAudio::HOVERED);
		buttonHoveredImage->Enable();
		buttonClickedImage->Disable();
		buttonIdleImage->Enable();
		if (buttonText && buttonTextShadow) {
			buttonText->SetFontColor(float4(1, 1, 1, 1));
			buttonTextShadow->Enable();
		}
		break;
	case ButtonState::CLICKED:
		PlayAudio(UIAudio::CLICKED);
		buttonHoveredImage->Disable();
		buttonClickedImage->Enable();
		buttonIdleImage->Enable();
		if (buttonText && buttonTextShadow) {
			buttonText->SetFontColor(float4(0, 0.1568, 0.2353, 1));
			buttonTextShadow->Disable();
		}
		break;
	}
	buttonState = newState;
}

void SpecialHoverButton::PlayAudio(UIAudio type) {
	if (audios[static_cast<int>(type)]) audios[static_cast<int>(type)]->Play();
}
