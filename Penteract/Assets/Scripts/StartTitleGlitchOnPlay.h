#pragma once

#include "Scripting/Script.h"

class GlitchyTitleController;

class StartTitleGlitchOnPlay : public Script {
	GENERATE_BODY(StartTitleGlitchOnPlay);

public:
	UID controllerObjUID = 0;

public:

	void Start() override;
	void Update() override;


	void OnButtonClick() override;
private:
	GlitchyTitleController* controller = nullptr;

};

