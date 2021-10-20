#pragma once

#include "Scripting/Script.h"

class GlitchyTitleController;
class GameObject;
class ComponentSelectable;

class StartTitleGlitchOnPlay : public Script {
	GENERATE_BODY(StartTitleGlitchOnPlay);

public:
	void Start() override;
	void Update() override;

	void OnButtonClick() override;
	void DoTransition();

public:
	UID controllerObjUID = 0;
	UID sceneUID = 0;
	UID parentCanvasUID = 0;

	/* To control the checkpoint*/
	int checkpointSelected = -1;
	int levelSelected = 0;

private:
	GameObject* parentCanvas = nullptr;
	GlitchyTitleController* controller = nullptr;
	ComponentSelectable* selectable = nullptr;
};

