#pragma once

#include "Scripting/Script.h"

class PlayerController;

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
	float4 buttonColorNotAvailable = float4(0.f / 255.f, 40.f / 255.f, 60.f / 255.f, 255.f / 255.f);
	float4 buttonColorAvailable = float4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 255.f / 255.f);
	float4 buttonTextColorNotAvailable = float4(0.f / 255.f, 0.f / 255.f, 0.f / 255.f, 255.f / 255.f);
	float4 buttonTextColorAvailable = float4(255.f / 255.f, 255.f / 255.f, 255.f / 255.f, 255.f / 255.f);
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

public:
	void UpdateCooldowns(float onimaruCooldown1, float onimaruCooldown2, float onimaruCooldown3, float fangCooldown1, float fangCooldown2, float fangCooldown3, float switchCooldown);
	void StartCharacterSwitch();
private:
	void UpdateVisualCooldowns(GameObject* canvas, int startingIt);
	void UpdateCommonSkillVisualCooldown();
	void ManageSwitch();
private:
	PlayerController* playerController = nullptr;
	GameObject* fangObj = nullptr;
	GameObject* onimaruObj = nullptr;


	//Skill HUD
	GameObject* fangSkillParent = nullptr;
	GameObject* onimaruSkillParent = nullptr;
	GameObject* switchSkillParent = nullptr;

	float switchTimer = 0.0f;

};

