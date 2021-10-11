#pragma once

#include "Scripting/Script.h"

class ComponentImage;
class ComponentSelectable;

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

	ComponentImage* buttonIdleImage = nullptr;
	ComponentImage* buttonHoveredImage = nullptr;
	ComponentImage* buttonClickedImage = nullptr;

private:
	ComponentSelectable* selectable = nullptr;
	enum class ButtonState { IDLE, HOVERED, CLICKED };
	ButtonState buttonState = ButtonState::IDLE;

private:
	void EnterButtonState(ButtonState newState);
};

