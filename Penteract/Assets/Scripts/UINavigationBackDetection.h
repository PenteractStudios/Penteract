#pragma once

#include "Scripting/Script.h"

class ComponentSelectable;

class UINavigationBackDetection : public Script
{
	GENERATE_BODY(UINavigationBackDetection);

public:

	void Start() override;
	void Update() override;

public:

	UID objectToEnableOnCancelUID = 0;
	UID objectToSelectOnEnableUID = 0;
	UID objectToDisableOnCancelUID = 0;

private:
	void ListenForCancelInput();
	void OnCancelInputPressed();
	void OnEnable() override;
private:

	GameObject* objectToEnableOnCancel = nullptr;
	GameObject* objectToDisableOnCancel = nullptr;
	ComponentSelectable* selectableToSelectOnEnable = nullptr;

};

