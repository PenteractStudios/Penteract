#pragma once

#include "Scripting/Script.h"

class PlayerController;
class ComponentTransform2D;

class HUDManager : public Script {
	GENERATE_BODY(HUDManager);

public:

	void Start() override;
	void Update() override;

public:
	enum class Cooldowns {
		FANG_SKILL_1,
		FANG_SKILL_2,
		FANG_SKILL_3,
		ONIMARU_SKILL_1,
		ONIMARU_SKILL_2,
		ONIMARU_SKILL_3,
		SWITCH_SKILL,
		TOTAL
	};

	enum class SwitchState {
		IDLE,
		PRE_COLLAPSE,
		COLLAPSE,
		DEPLOY,
		POST_DEPLOY
	};

	UID playerObjectUID = 0;

	//Skill HUD

	//Texts
	float4 buttonColorNotAvailable = float4(0.f / 255.f, 40.f / 255.f, 60.f / 255.f, 255.f / 255.f);
	float4 buttonColorAvailable = float4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 255.f / 255.f);
	float4 buttonTextColorNotAvailable = float4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 255.f / 255.f);
	float4 buttonTextColorAvailable = float4(0.f / 255.f, 40.f / 255.f, 60.f / 255.f, 255.f / 255.f);

	//Skills
	float4 skillColorNotAvailable = float4(0.f / 255.f, 93.f / 255.f, 145.f / 255.f, 255.f / 255.f);
	float4 skillColorAvailable = float4(0.f / 255.f, 177.f / 255.f, 227.f / 255.f, 255.f / 255.f);
	float4 switchSkillColorNotAvailable = float4(133.f / 255.f, 243.f / 255.f, 196.f / 255.f, 150.f / 255.f);
	float4 switchSkillColorAvailable = float4(133.f / 255.f, 243.f / 255.f, 196.f / 255.f, 255.f / 255.f);
	float4 switchPictoColorNotInUse = float4(133.f / 255.f, 243.f / 255.f, 196.f / 255.f, 255.f / 255.f);
	float4 switchPictoColorInUse = float4(0.f / 255.f, 0.f / 255.f, 0.f / 255.f, 255.f / 255.f);
	UID fangSkillParentUID = 0;
	UID onimaruSkillParentUID = 0;
	UID switchSkillParentUID = 0;

	float cooldowns[static_cast<int>(Cooldowns::TOTAL)];
	float3 cooldownTransformOriginalPositions[static_cast<int>(Cooldowns::TOTAL)];

	std::vector<GameObject*> skillsFang;
	std::vector<GameObject*> skillsOni;

	float switchPreCollapseMovementTime = 0.2f;
	float switchCollapseMovementTime = 0.4f;
	float switchDeployMovementTime = 0.4f;
	float switchPostDeployMovementTime = 0.2f;
	float switchExtraOffset = 20.0f;
	SwitchState switchState = SwitchState::IDLE;

	float switchColorTimer = 0.0f;
	float switchColorTotalTime = 2.0f;
	bool switchColorIncreasing = true;

	float switchSymbolRotationTime = 6.0f;

	// Health HUD
	UID fangHealthParentUID = 0;
	UID onimaruHealthParentUID = 0;

public:
	void UpdateCooldowns(float onimaruCooldown1, float onimaruCooldown2, float onimaruCooldown3, float fangCooldown1, float fangCooldown2, float fangCooldown3, float switchCooldown);
	void UpdateHealth(float fangHealth, float onimaruHealth);
	void HealthRegeneration(float health, float healthRecovered);
	void StartCharacterSwitch();
private:
	void UpdateVisualCooldowns(GameObject* canvas, int startingIt); //Update visual cooldown on all abilities of a given character
	void UpdateCommonSkillVisualCooldown(); //Update visual cooldown on switch ability
	void ManageSwitch();	//This method manages visual effects regarding the Switching of characters (UI WISE) as well 
							//as the color changin and rotation of the picto for the switch icon

private:
	
	PlayerController* playerController = nullptr;
	GameObject* fangObj = nullptr;
	GameObject* onimaruObj = nullptr;

	//Skill HUD
	GameObject* fangSkillParent = nullptr;
	GameObject* onimaruSkillParent = nullptr;
	GameObject* switchSkillParent = nullptr;
	ComponentTransform2D* switchShadeTransform = nullptr;
	float switchTimer = 0.0f;

	// Health HUD
	GameObject* fangHealthParent = nullptr;
	GameObject* onimaruHealthParent = nullptr;

};

