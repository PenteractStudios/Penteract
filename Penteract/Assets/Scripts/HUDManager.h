#pragma once

#include "Scripting/Script.h"

class HUDManager : public Script {
	GENERATE_BODY(HUDManager);

public:

	void Start() override;
	void Update() override;

public:
	//Skill HUD
	float4 buttonColorNotAvailable = float4(0, 40, 60, 255);
	float4 buttonColorAvailable = float4(255, 255, 255, 255);
	float4 buttonTextColorAvailable = float4(0, 0, 0, 255);
	float4 buttonTextColorAvailable = float4(255, 255, 255, 255);
	float4 skillColorNotAvailable = float4(0, 93, 145, 255);
	float4 skillColorAvailable = float4(0, 177, 227, 255);
	float4 switchSkillColorNotAvailable = float4(133, 243, 196, 150);
	float4 switchSkillColorAvailable = float4(133, 243, 196, 255);
	float4 switchPictoColorNotInUse = float4(133, 243, 196, 255);
	float4 switchPictoColorInUse = float4(0, 0, 0, 255);
	UID fangSkillParentUID = 0;
	UID onimaruSkillParentUID = 0;
	UID switchSkillParentUID = 0;


private:
	//Skill HUD
	GameObject* fangSkillParent = nullptr;
	GameObject* onimaruSkillParent = nullptr;
	GameObject* switchSkillParent = nullptr;

};

