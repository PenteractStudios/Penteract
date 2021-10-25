#pragma once

#include "Scripting/Script.h"
#include "UIAudioType.h"

class ComponentImage;
class ComponentSelectable;
class ComponentText;
class ComponentAudioSource;

class SpecialHoverButton : public Script {
	GENERATE_BODY(SpecialHoverButton);

public:

	void Start() override;
	void Update() override;
	void OnButtonClick() override;
	void OnDisable() override;
	void PlayAudio(UIAudio type);

public:

	UID buttonIdleImageObjUID = 0;
	UID buttonHoveredImageObjUID = 0;
	UID buttonClickedImageObjUID = 0;
	UID buttonTextWhiteObjUID = 0;
	UID buttonTextShadowObjUID = 0;
	UID audioSourcesUID = 0;

	ComponentImage* buttonIdleImage = nullptr;
	ComponentImage* buttonHoveredImage = nullptr;
	ComponentImage* buttonClickedImage = nullptr;
	ComponentText* buttonText = nullptr;
	ComponentText* buttonTextShadow = nullptr;

private:
	ComponentSelectable* selectable = nullptr;
	enum class ButtonState { IDLE, HOVERED, CLICKED };
	ButtonState buttonState = ButtonState::IDLE;
	ComponentAudioSource* audios[static_cast<int>(UIAudio::TOTAL)] = { nullptr };

private:
	void EnterButtonState(ButtonState newState);
};

