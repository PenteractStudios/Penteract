#pragma once
#pragma once

#include "Scripting/Script.h"

class GameObject;
class ComponentText;

#define MAX_HEALTH 7
#define LOW_HEALTH_WARNING 2

enum Cooldowns {
	FANG_SKILL_1,
	FANG_SKILL_2,
	FANG_SKILL_3,
	ONIMARU_SKILL_1,
	ONIMARU_SKILL_2,
	ONIMARU_SKILL_3,
	SWITCH_SKILL,
	TOTAL
};

class HUDController : public Script {
	GENERATE_BODY(HUDController);

public:

	void Start() override;
	void Update() override;
	void UpdateCooldowns(float onimaruCooldown1, float onimaruCooldown2, float onimaruCooldown3,
		float fangCooldown1, float fangCooldown2, float fangCooldown3,
		float switchCooldown);

	void UpdateHP(float currentHp, float altHp);
	void UpdateDurableHPLoss(GameObject* targetCanvas);
	void ChangePlayerHUD(int fangLives, int oniLives);
	void HealthRegeneration(float currentHp, float hpRecovered);
	void ResetHealthFill(float currentHp);
	
	void UpdateScore(int score_);

public:
	UID fangUID = 0;
	UID onimaruUID = 0;

	UID fangMainCanvasUID = 0;
	UID onimaruMainCanvasUID = 0;

	UID fangSkillsMainCanvasUID = 0;
	UID onimaruSkillsMainCanvasUID = 0;

	UID fangSkillsSecondCanvasUID = 0;
	UID onimaruSkillsSecondCanvasUID = 0;

	UID fangHealthMainCanvasUID = 0;
	UID onimaruHealthMainCanvasUID = 0;

	UID fangHealthSecondCanvasUID = 0;
	UID onimaruHealthSecondCanvasUID = 0;

	UID lowHealthWarningEffectUID = 0;

	UID swapingSkillCanvasUID = 0;

	UID scoreTextUID = 0;
	
	float timeToFadeDurableHealthFeedbackInternal = 2.0f;

private:
	void UpdateComponents();
	void UpdateCommonSkill();
	void UpdateFangCooldowns(GameObject* fangSkillCanvas);
	void UpdateOnimaruCooldowns(GameObject* onimaruSkillCanvas);
	void UpdateCanvasHP(GameObject* targetCanvas, int health, bool darkened);
	void OnHealthLost(GameObject* targetCanvas, int health);
	void StopHealthLostInstantEffects(GameObject* targetCanvas);
	void LoadHealthFeedbackStates(GameObject* targetCanvas, int health);

private:
	GameObject* fang = nullptr;
	GameObject* onimaru = nullptr;

	GameObject* fangCanvas = nullptr;
	GameObject* onimaruCanvas = nullptr;

	GameObject* fangSkillsMainCanvas = nullptr;
	GameObject* onimaruSkillsMainCanvas = nullptr;
	GameObject* fangSkillsSecondCanvas = nullptr;
	GameObject* onimaruSkillsSecondCanvas = nullptr;

	GameObject* fangHealthMainCanvas = nullptr;
	GameObject* onimaruHealthMainCanvas = nullptr;
	GameObject* fangHealthSecondCanvas = nullptr;
	GameObject* onimaruHealthSecondCanvas = nullptr;

	GameObject* lowHealthWarningEffect = nullptr;

	GameObject* swapingSkillCanvas = nullptr;

	float cooldowns[Cooldowns::TOTAL];

	/* COLORS */

	const float4 colorMagenta = float4(236, 60, 137, 255) / 255;
	const float4 colorWhite = float4(255, 255, 255, 255) / 255;

	const float4 colorMagentaDarkened = float4(236, 60, 137, 128) / 255;
	const float4 colorWhiteDarkened = float4(255, 255, 255, 128) / 255;


	int prevLivesFang = MAX_HEALTH;
	int prevLivesOni = MAX_HEALTH;

	bool lowHPWarningActive = false;

	float timeToFadeDurableHealthFeedback = 0.0f;

	float remainingTimesFang[MAX_HEALTH] = { 0,0,0,0,0,0,0 };
	float remainingTimesOni[MAX_HEALTH] = { 0,0,0,0,0,0,0 };

	std::vector<int>remainingTimeActiveIndexesFang;
	std::vector<int>remainingTimeActiveIndexesOni;

	ComponentText* scoreText = nullptr;
	int score = 0;
};

