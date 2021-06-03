#pragma once
#pragma once

#include "Scripting/Script.h"

class GameObject;
class ComponentText;
class AbilityRefreshEffect;

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
	static void HealthRegeneration(float currentHp, float hpRecovered);
	static void ResetHealthFill(float currentHp);
	static void ChangePlayerHUD(int fangLives, int oniLives);
	void UpdateScore(int score_);
	static void SetCooldownRetreival(Cooldowns cooldown);
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

	float timeToFadeDurableHealthFeedbackInternal;
	static bool abilityCoolDownsRetreived[];

private:
	static void UpdateComponents();
	static void UpdateCommonSkill();
	static void UpdateFangCooldowns(GameObject* fangSkillCanvas, bool isMain);
	static void UpdateOnimaruCooldowns(GameObject* onimaruSkillCanvas, bool isMain);
	static void UpdateCanvasHP(GameObject* targetCanvas, int health, bool darkened);
	static void OnHealthLost(GameObject* targetCanvas, int health);
	static void AbilityCoolDownEffectCheck(Cooldowns cooldown, GameObject* canvas);
	static void PlayCoolDownEffect(AbilityRefreshEffect* effect, Cooldowns cooldown);
	static void StopHealthLostInstantEffects(GameObject* targetCanvas);
	static void LoadHealthFeedbackStates(GameObject* targetCanvas, int health);

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

	static GameObject* lowHealthWarningEffect;

	static GameObject* swapingSkillCanvas;



	static std::array<float, Cooldowns::TOTAL> cooldowns;

	/* COLORS */

	static const float4 colorMagenta;
	static const float4 colorWhite;

	static const float4 colorMagentaDarkened;
	static const float4 colorWhiteDarkened;


	static int prevLivesFang;
	static int prevLivesOni;

	static bool lowHPWarningActive;

	static float remainingTimesFang[];
	static float remainingTimesOni[];

	static std::vector<int>remainingTimeActiveIndexesFang;
	static std::vector<int>remainingTimeActiveIndexesOni;

	ComponentText* scoreText = nullptr;
	int score = 0;
};

