#pragma once

#include "Scripting/Script.h"

class ComponentImage;
class ComponentSelectable;
class ComponentText;

class SpecialHoverButton : public Script {
	GENERATE_BODY(SpecialHoverButton);

public:

	void Start() override;
	void Update() override;
	void OnButtonClick() override;

public:

	UID buttonIdleImageObjUID = 0;
	UID buttonHoveredImageObjUID = 0;
	UID buttonClickedImageObjUID = 0;
	UID buttonTextWhiteObjUID = 0;
	UID buttonTextShadowObjUID = 0;

	ComponentImage* buttonIdleImage = nullptr;
	ComponentImage* buttonHoveredImage = nullptr;
	ComponentImage* buttonClickedImage = nullptr;
	ComponentText* buttonText = nullptr;
	ComponentText* buttonTextShadow = nullptr;

private:
	ComponentSelectable* selectable = nullptr;
	enum class ButtonState { IDLE, HOVERED, CLICKED };
	ButtonState buttonState = ButtonState::IDLE;

private:
	void EnterButtonState(ButtonState newState);
};

