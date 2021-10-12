#pragma once

#include "Scripting/Script.h"

#include "GameObject.h"

class ComponentSelectable;
class ComponentVideo;

class SwapPanels : public Script {
	GENERATE_BODY(SwapPanels);

public:

	void Start() override;
	void Update() override;
	void OnButtonClick() override;
	void DoSwapPanels();
public:
	UID targetUID = 0;
	UID currentUID = 0;
	UID optionalVideoUID = 0;
	GameObject* target = nullptr;
	GameObject* current = nullptr;

private:
	ComponentSelectable* selectable = nullptr;
	ComponentVideo* video = nullptr;
	
};

