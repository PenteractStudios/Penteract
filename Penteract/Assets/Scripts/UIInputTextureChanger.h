#pragma once

#include "Scripting/Script.h"

class ComponentImage;

class UIInputTextureChanger : public Script {
	GENERATE_BODY(UIInputTextureChanger);

public:

	void Start() override;
	void Update() override;

	ComponentImage* GetActiveImage();

private:
	void EnableGamepadObj();
	void EnableKeyboardObj();

public:
	UID gameControllerKeyImageObjUID = 0;
	UID keyboardKeyImageObjUID = 0;

private:
	bool lastSerializedGamepadOn = false;

	GameObject* gameControllerKeyImageObj = nullptr;
	GameObject* keyboardKeyImageObj = nullptr;
};

