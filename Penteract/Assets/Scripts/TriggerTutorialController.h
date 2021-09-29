#pragma once

#include "Scripting/Script.h"

class GameObject;
class TriggerTutorialController : public Script
{
	GENERATE_BODY(TriggerTutorialController);

public:

	void Start() override;
	void Update() override;

public:
	UID tutorialFang1UID = 0;
	UID tutorialFang2UID = 0;
	UID tutorialFang3UID = 0;
	UID tutorialFangUltiUID = 0;
	UID tutorialOnimaru1UID = 0;
	UID tutorialOnimaru2UID = 0;
	UID tutorialOnimaruUltiUID = 0;

private:
	GameObject* tutorialFang1 = nullptr;
	GameObject* tutorialFang2 = nullptr;
	GameObject* tutorialFang3 = nullptr;
	GameObject* tutorialFangUlti = nullptr;
	GameObject* tutorialOnimaru1 = nullptr;
	GameObject* tutorialOnimaru2 = nullptr;
	GameObject* tutorialOnimaruUlti = nullptr;
};

