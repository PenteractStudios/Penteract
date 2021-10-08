#pragma once

#include "Scripting/Script.h"

#include "GameObject.h"

class ComponentSelectable;

class SwapPanels : public Script {
	GENERATE_BODY(SwapPanels);

public:

	void Start() override;
	void Update() override;
	void OnButtonClick() override;
	void DoSwapPanels();
public:
	UID targetUID;
	UID currentUID;
	GameObject* target;
	GameObject* current;

private:
	ComponentSelectable* selectable = nullptr;
	
};

