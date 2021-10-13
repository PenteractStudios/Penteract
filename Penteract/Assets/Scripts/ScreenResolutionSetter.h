#pragma once

#include "Scripting/Script.h"

class ComponentText;
class ComponentSelectable;

 /// <summary>
 /// This class is meant to modify externs delcared inside ScreenResolution confirmer whenever Button is pressed, as well as setting the text (if found)
 /// </summary>
 class ScreenResolutionSetter : public Script {
	GENERATE_BODY(ScreenResolutionSetter);

public:

	void Start() override;
	void Update() override;
	void OnButtonClick() override;

public:
	bool increasing = false;
	UID textObjectID = 0;

private:
	void IncreaseResolution(unsigned multiplier);
	void UpdateResolution();
private:
	ComponentText* text = nullptr;
	ComponentSelectable* selectable = nullptr;

};

