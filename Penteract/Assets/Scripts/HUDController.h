#pragma once
#pragma once

#include "Scripting/Script.h"

class GameObject;
class ComponentText;

static enum Cooldowns {
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
	static void UpdateCooldowns(float onimaruCooldown1, float onimaruCooldown2, float onimaruCooldown3,
		float fangCooldown1, float fangCooldown2, float fangCooldown3,
		float switchCooldown);

	static void UpdateHP(float currentHp, float altHp);
	static void UpdateDurableHPLoss(GameObject* targetCanvas);
	static void ChangePlayerHUD();

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

	UID swapingSkillCanvasUID = 0;

	UID fangSkillCooldownEffectCanvasUID = 0;
	UID onimaruSkillCooldownEffectCanvasUID = 0;
	UID swappingSkillCooldownEffectCanvasUID = 0;


	UID scoreTextUID = 0;
	float timeToFadeDurableHealthFeedbackInternal;
private:
	static void UpdateComponents();
	static void UpdateCommonSkill();
	static void UpdateFangCooldowns(GameObject* fangSkillCanvas, GameObject* fangCoolDownEffectCanvas, bool isMain);
	static void UpdateOnimaruCooldowns(GameObject* onimaruSkillCanvas, GameObject* onimaruCoolDownEffectCanvas, bool isMain);
	static void UpdateCanvasHP(GameObject* targetCanvas, int health, bool darkened);
	static void OnHealthLost(GameObject* targetCanvas, int health);
	//static void PlayFangCoolDownFinishedEffect(GameObject* fangSkillCanvas, int enumIndex);
	//static void PlayOnimaruCoolDownFinishedEffect(GameObject* onimaruSkillCanvas, int enumIndex);
	//static void PlayCommonSkillCoolDownFinishedEffect(GameObject* commonSkillCanvas, int enumIndex);

	static void AbilityCoolDownEffectCheck(Cooldowns cooldown);


private:
	static float timeToFadeDurableHealthFeedback;
	static GameObject* fang;
	static GameObject* onimaru;

	static GameObject* fangCanvas;
	static GameObject* onimaruCanvas;

	static GameObject* fangSkillsMainCanvas;
	static GameObject* onimaruSkillsMainCanvas;
	static GameObject* fangSkillsSecondCanvas;
	static GameObject* onimaruSkillsSecondCanvas;

	static GameObject* fangHealthMainCanvas;
	static GameObject* onimaruHealthMainCanvas;
	static GameObject* fangHealthSecondCanvas;
	static GameObject* onimaruHealthSecondCanvas;

	static GameObject* swapingSkillCanvas;

	//Skill Cooldown Effects
	static GameObject* fangSkillCooldownEffectCanvas;
	static GameObject* onimaruSkillCooldownEffectCanvas;
	static GameObject* swappingSkillCooldownEffectCanvas;


	static std::array<float, Cooldowns::TOTAL> cooldowns;

	/* COLORS */

	static const float4 colorMagenta;
	static const float4 colorWhite;

	static const float4 colorMagentaDarkened;
	static const float4 colorWhiteDarkened;

	static int prevLives;
	static float remainingTimes[];
	static bool abilityCoolDownsRetreived[];
	static std::vector<int>remainingTimeActiveIndexes;

	ComponentText* scoreText = nullptr;
	int score = 0;
};

