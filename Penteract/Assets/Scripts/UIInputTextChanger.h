#pragma once

#include "Scripting/Script.h"
#include <string>

class ComponentText;

class UIInputTextChanger : public Script
{
	GENERATE_BODY(UIInputTextChanger);

public:

	void Start() override;
	void Update() override;

private:
	void SetTextToGamepad();
	void SetTextToKeyboard();

public:

	std::string gameControllerText = "";
	std::string keyboardText = "";

private:
	bool lastSerializedGamepadOn = false;
	ComponentText* textToModify = nullptr;

};

