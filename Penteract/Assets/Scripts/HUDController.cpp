#include "HUDController.h"

#include "GameObject.h"
#include "Components/UI/ComponentImage.h"
#include "HealthLostInstantFeedback.h"
#include "LowHPWarning.h"
#include "GameplaySystems.h"

// We should get this from the character class
#define MAX_HEALTH 7
#define LOW_HEALTH_WARNING 2

EXPOSE_MEMBERS(HUDController) {
	// Add members here to expose them to the engine. Example:
	MEMBER(MemberType::GAME_OBJECT_UID, fangMainCanvasUID),
		MEMBER(MemberType::GAME_OBJECT_UID, onimaruMainCanvasUID),
		MEMBER(MemberType::GAME_OBJECT_UID, fangSkillsMainCanvasUID),
		MEMBER(MemberType::GAME_OBJECT_UID, onimaruSkillsMainCanvasUID),
		MEMBER(MemberType::GAME_OBJECT_UID, fangSkillsSecondCanvasUID),
		MEMBER(MemberType::GAME_OBJECT_UID, onimaruSkillsSecondCanvasUID),
		MEMBER(MemberType::GAME_OBJECT_UID, fangHealthMainCanvasUID),
		MEMBER(MemberType::GAME_OBJECT_UID, onimaruHealthMainCanvasUID),
		MEMBER(MemberType::GAME_OBJECT_UID, fangHealthSecondCanvasUID),
		MEMBER(MemberType::GAME_OBJECT_UID, onimaruHealthSecondCanvasUID),
		MEMBER(MemberType::GAME_OBJECT_UID, swapingSkillCanvasUID),
		MEMBER(MemberType::GAME_OBJECT_UID, fangUID),
		MEMBER(MemberType::GAME_OBJECT_UID, onimaruUID),
		MEMBER(MemberType::GAME_OBJECT_UID, scoreTextUID),
		MEMBER(MemberType::GAME_OBJECT_UID, lowHealthWarningEffectUID),
		MEMBER(MemberType::FLOAT, timeToFadeDurableHealthFeedbackInternal)
};

GENERATE_BODY_IMPL(HUDController);

GameObject* HUDController::fangCanvas = nullptr;
GameObject* HUDController::onimaruCanvas = nullptr;

GameObject* HUDController::fang = nullptr;
GameObject* HUDController::onimaru = nullptr;

GameObject* HUDController::fangSkillsMainCanvas = nullptr;
GameObject* HUDController::onimaruSkillsMainCanvas = nullptr;
GameObject* HUDController::fangSkillsSecondCanvas = nullptr;
GameObject* HUDController::onimaruSkillsSecondCanvas = nullptr;

GameObject* HUDController::fangHealthMainCanvas = nullptr;
GameObject* HUDController::onimaruHealthMainCanvas = nullptr;
GameObject* HUDController::fangHealthSecondCanvas = nullptr;
GameObject* HUDController::onimaruHealthSecondCanvas = nullptr;
GameObject* HUDController::swapingSkillCanvas = nullptr;
GameObject* HUDController::lowHealthWarningEffect = nullptr;

std::array<float, Cooldowns::TOTAL> HUDController::cooldowns;


const float4 HUDController::colorMagenta = float4(236, 60, 137, 255) / 255;
const float4 HUDController::colorWhite = float4(255, 255, 255, 255) / 255;

const float4 HUDController::colorMagentaDarkened = float4(236, 60, 137, 128) / 255;
const float4 HUDController::colorWhiteDarkened = float4(255, 255, 255, 128) / 255;
int HUDController::prevLivesFang = MAX_HEALTH;
int HUDController::prevLivesOni = MAX_HEALTH;
bool HUDController::lowHPWarningActive = false;

float HUDController::remainingTimesFang[MAX_HEALTH] = { 0,0,0,0,0,0,0 };
float HUDController::remainingTimesOni[MAX_HEALTH] = { 0,0,0,0,0,0,0 };

float HUDController::timeToFadeDurableHealthFeedback = 2.0f;
std::vector<int>HUDController::remainingTimeActiveIndexesOni;
std::vector<int>HUDController::remainingTimeActiveIndexesFang;

void HUDController::Start() {
	//Resetting preLives and remainingTimes just in case
	prevLivesFang = prevLivesOni = MAX_HEALTH;
	for (int i = 0; i < MAX_HEALTH; i++) {
		remainingTimesFang[i] = remainingTimesOni[i] = 0;
	}
	timeToFadeDurableHealthFeedback = timeToFadeDurableHealthFeedbackInternal;

	fangCanvas = GameplaySystems::GetGameObject(fangMainCanvasUID);
	onimaruCanvas = GameplaySystems::GetGameObject(onimaruMainCanvasUID);

	fangSkillsMainCanvas = GameplaySystems::GetGameObject(fangSkillsMainCanvasUID);
	onimaruSkillsMainCanvas = GameplaySystems::GetGameObject(onimaruSkillsMainCanvasUID);
	fangSkillsSecondCanvas = GameplaySystems::GetGameObject(fangSkillsSecondCanvasUID);
	onimaruSkillsSecondCanvas = GameplaySystems::GetGameObject(onimaruSkillsSecondCanvasUID);

	fangHealthMainCanvas = GameplaySystems::GetGameObject(fangHealthMainCanvasUID);
	onimaruHealthMainCanvas = GameplaySystems::GetGameObject(onimaruHealthMainCanvasUID);
	fangHealthSecondCanvas = GameplaySystems::GetGameObject(fangHealthSecondCanvasUID);
	onimaruHealthSecondCanvas = GameplaySystems::GetGameObject(onimaruHealthSecondCanvasUID);
	swapingSkillCanvas = GameplaySystems::GetGameObject(swapingSkillCanvasUID);

	fang = GameplaySystems::GetGameObject(fangUID);
	onimaru = GameplaySystems::GetGameObject(onimaruUID);

	lowHealthWarningEffect = GameplaySystems::GetGameObject(lowHealthWarningEffectUID);

	GameObject* text = GameplaySystems::GetGameObject(scoreTextUID);
	if (text) scoreText = text->GetComponent<ComponentText>();

	if (onimaru && onimaruCanvas && fangCanvas && fang) {
		onimaru->Disable();
		onimaruCanvas->Disable();
		fangCanvas->Enable();
		fang->Enable();
	}


	cooldowns[Cooldowns::FANG_SKILL_1] = 0;
	cooldowns[Cooldowns::FANG_SKILL_2] = 0;
	cooldowns[Cooldowns::FANG_SKILL_3] = 0;
	cooldowns[Cooldowns::ONIMARU_SKILL_1] = 0;
	cooldowns[Cooldowns::ONIMARU_SKILL_2] = 0;
	cooldowns[Cooldowns::ONIMARU_SKILL_3] = 0;
	cooldowns[Cooldowns::SWITCH_SKILL] = 0;
}

void HUDController::Update() {
	if (!fangCanvas || !onimaruCanvas) return;
}

void HUDController::ChangePlayerHUD() {
	if (!fang || !onimaru) return;

	if (!fang->IsActive()) {
		fangCanvas->Disable();
		onimaruCanvas->Enable();
	} else {
		onimaruCanvas->Disable();
		fangCanvas->Enable();
	}

	remainingTimeActiveIndexesFang.clear();
	remainingTimeActiveIndexesOni.clear();
	for (int i = 0; i < MAX_HEALTH; i++) {
		remainingTimesFang[i] = 0;
		remainingTimesOni[i] = 0;
	}
}

void HUDController::HealthRegeneration(float currentHp, float hpRecovered) {
	if (fang->IsActive()) {
		const GameObject* healthSlot = onimaruHealthSecondCanvas->GetChildren()[currentHp];
		if (healthSlot) {
			for (GameObject* healthComponents : healthSlot->GetChildren()) {
				ComponentImage* healthFill = healthComponents->GetComponent<ComponentImage>();
				if (healthFill->IsFill()) {
					healthFill->SetFillValue(hpRecovered);
				}
			}
		}
	}
	else {
		const GameObject* healthSlot = fangHealthSecondCanvas->GetChildren()[currentHp];
		if (healthSlot) {
			for (GameObject* healthComponents : healthSlot->GetChildren()) {
				ComponentImage* healthFill = healthComponents->GetComponent<ComponentImage>();
				if (healthFill->IsFill()) {
					healthFill->SetFillValue(hpRecovered);
				}
			}
		}
	}
}

void HUDController::ResetHealthFill(float currentHp) {
	if (fang->IsActive()) {
		for (int pos = currentHp; pos < MAX_HEALTH; ++pos) {
			const GameObject* healthSlot = onimaruHealthSecondCanvas->GetChildren()[pos];
			for (GameObject* healthComponents : healthSlot->GetChildren()) {
				ComponentImage* healthFill = healthComponents->GetComponent<ComponentImage>();
				if (healthFill->IsFill()) {
					healthFill->SetFillValue(0.0f);
				}
			}
		}
	}
	else if (onimaru->IsActive()) {
		for (int pos = currentHp; pos < MAX_HEALTH; ++pos) {
			const GameObject* healthSlot = fangHealthSecondCanvas->GetChildren()[pos];
			for (GameObject* healthComponents : healthSlot->GetChildren()) {
				ComponentImage* healthFill = healthComponents->GetComponent<ComponentImage>();
				if (healthFill->IsFill()) {
					healthFill->SetFillValue(0.0f);
				}
			}
		}
	}
}

void HUDController::UpdateScore(int score_) {
	score += score_;
	if (scoreText) scoreText->SetText(std::to_string(score));
}

void HUDController::UpdateCooldowns(float onimaruCooldown1, float onimaruCooldown2, float onimaruCooldown3, float fangCooldown1, float fangCooldown2, float fangCooldown3, float switchCooldown) {

	cooldowns[Cooldowns::FANG_SKILL_1] = fangCooldown1;
	cooldowns[Cooldowns::FANG_SKILL_2] = fangCooldown2;
	cooldowns[Cooldowns::FANG_SKILL_3] = fangCooldown3;
	cooldowns[Cooldowns::ONIMARU_SKILL_1] = onimaruCooldown1;
	cooldowns[Cooldowns::ONIMARU_SKILL_2] = onimaruCooldown2;
	cooldowns[Cooldowns::ONIMARU_SKILL_3] = onimaruCooldown3;
	cooldowns[Cooldowns::SWITCH_SKILL] = switchCooldown;

	UpdateComponents();
}

void HUDController::UpdateHP(float currentHp, float altHp) {
	if (!fang || !onimaru) return;
	if (fang->IsActive()) {
		UpdateCanvasHP(fangHealthMainCanvas, currentHp, false);
		//UpdateCanvasHP(onimaruHealthSecondCanvas, altHp, true);
	} else {
		UpdateCanvasHP(onimaruHealthMainCanvas, currentHp, false);
		//UpdateCanvasHP(fangHealthSecondCanvas, altHp, true);
	}

	bool activateEffect = currentHp <= LOW_HEALTH_WARNING || altHp <= LOW_HEALTH_WARNING ? true : false;

	if (lowHealthWarningEffect) {
		if (activateEffect) {
			if (!lowHPWarningActive) {
				LowHPWarning* effect = GET_SCRIPT(lowHealthWarningEffect, LowHPWarning);
				effect->Play();
				lowHPWarningActive = true;
			}
		} else {
			if (lowHPWarningActive) {
				LowHPWarning* effect = GET_SCRIPT(lowHealthWarningEffect, LowHPWarning);
				effect->Stop();
				lowHPWarningActive = false;
			}
		}
	}
}

void HUDController::UpdateDurableHPLoss(GameObject* targetCanvas) {
	bool isFang = (targetCanvas->GetID() == fangHealthMainCanvas->GetID());

	std::vector<int>& indexes = isFang ? remainingTimeActiveIndexesFang : remainingTimeActiveIndexesOni;
	float* times = isFang ? remainingTimesFang : remainingTimesOni;

	if (indexes.size() > 0) {
		std::vector<std::vector<int>::iterator>vectorIndexesToRemove;

		for (std::vector<int>::iterator it = indexes.begin(); it != indexes.end(); ++it) {
			times[(*it)] -= Time::GetDeltaTime();

			if (times[(*it)] <= 0) {
				times[(*it)] = 0;
				vectorIndexesToRemove.push_back(it);
			}

			//We need a GetColor
			float delta = times[(*it)] / timeToFadeDurableHealthFeedback;


			//TODO GetChildren()[(*it)] Becomes GetChildren()[(*it)].GetChildren()[1] //Because BG -> HP -> Effect (hierarchy)
			ComponentImage* image = targetCanvas->GetChildren()[(*it)]->GetComponent<ComponentImage>();

			//We need access to image->SetAlphaTransparency
			image->SetColor(float4(1, 1, 1, delta));
		}

		for (int i = 0; i < vectorIndexesToRemove.size(); i++) {
			indexes.erase(vectorIndexesToRemove[i]);
		}

	}
}

void HUDController::UpdateComponents() {

	if (!fang || !onimaru) return;

	if (fangHealthMainCanvas) {
		if (fangHealthMainCanvas->IsActive()) {
			UpdateDurableHPLoss(fangHealthMainCanvas);
		}
	}

	if (onimaruHealthMainCanvas) {
		if (onimaruHealthMainCanvas->IsActive()) {
			UpdateDurableHPLoss(onimaruHealthMainCanvas);
		}
	}

	if (!fangSkillsMainCanvas || !onimaruSkillsMainCanvas || !fangSkillsSecondCanvas || !onimaruSkillsSecondCanvas) return;
	if (!fangHealthMainCanvas || !onimaruHealthMainCanvas || !fangHealthSecondCanvas || !onimaruHealthSecondCanvas) return;
	if (!swapingSkillCanvas) return;
	// Update all cooldowns
	if (fang->IsActive()) {
		UpdateFangCooldowns(fangSkillsMainCanvas);
		UpdateOnimaruCooldowns(onimaruSkillsSecondCanvas);
		UpdateCommonSkill();
	} else if (onimaru->IsActive()) {
		UpdateOnimaruCooldowns(onimaruSkillsMainCanvas);
		UpdateFangCooldowns(fangSkillsSecondCanvas);
		UpdateCommonSkill();
	}
}

void HUDController::UpdateCommonSkill() {
	std::vector<GameObject*> children = swapingSkillCanvas->GetChildren();
	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it) {
		ComponentImage* image = (*it)->GetComponent<ComponentImage>();
		if (image) {
			if (image->IsFill()) {
				image->SetFillValue(cooldowns[Cooldowns::SWITCH_SKILL]);
			}
		}
	}
}

void HUDController::UpdateFangCooldowns(GameObject* fangSkillCanvas) {
	std::vector<GameObject*> children = fangSkillCanvas->GetChildren();
	int skill = FANG_SKILL_1;
	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it) {
		std::vector<GameObject*> skills = (*it)->GetChildren();
		for (std::vector<GameObject*>::iterator itSkills = skills.begin(); itSkills != skills.end(); ++itSkills) {
			ComponentImage* image = (*itSkills)->GetComponent<ComponentImage>();
			if (image) {
				if (image->IsFill()) {
					switch (skill) {
					case Cooldowns::FANG_SKILL_1:
						image->SetFillValue(cooldowns[Cooldowns::FANG_SKILL_1]);
						break;
					case Cooldowns::FANG_SKILL_2:
						image->SetFillValue(cooldowns[Cooldowns::FANG_SKILL_2]);
						break;
					case Cooldowns::FANG_SKILL_3:
						image->SetFillValue(cooldowns[Cooldowns::FANG_SKILL_3]);
						break;
					default:
						break;
					}
				}
			}
		}
		++skill;
	}
}

void HUDController::UpdateOnimaruCooldowns(GameObject* onimaruSkillCanvas) {
	std::vector<GameObject*> children = onimaruSkillCanvas->GetChildren();
	int skill = ONIMARU_SKILL_1;
	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it) {
		std::vector<GameObject*> skills = (*it)->GetChildren();
		for (std::vector<GameObject*>::iterator itSkills = skills.begin(); itSkills != skills.end(); ++itSkills) {
			ComponentImage* image = (*itSkills)->GetComponent<ComponentImage>();
			if (image) {
				if (image->IsFill()) {
					switch (skill) {
					case Cooldowns::ONIMARU_SKILL_1:
						image->SetFillValue(cooldowns[Cooldowns::ONIMARU_SKILL_1]);
						break;
					case Cooldowns::ONIMARU_SKILL_2:
						image->SetFillValue(cooldowns[Cooldowns::ONIMARU_SKILL_2]);
						break;
					case Cooldowns::ONIMARU_SKILL_3:
						image->SetFillValue(cooldowns[Cooldowns::ONIMARU_SKILL_3]);
						break;
					default:
						break;
					}
				}
			}
		}
		++skill;
	}
}

void HUDController::OnHealthLost(GameObject* targetCanvas, int health) {

	//TODO GetChildren()[(*it)] Becomes GetChildren()[(*it)].GetChildren()[2] //Because BG -> HP -> Effect (hierarchy)

	bool isFang = (targetCanvas->GetID() == fangHealthMainCanvas->GetID());

	GameObject* obj = targetCanvas->GetChildren()[health];


	if (isFang) {
		remainingTimeActiveIndexesFang.push_back(health);
		remainingTimesFang[health] = timeToFadeDurableHealthFeedback;
	} else {
		remainingTimeActiveIndexesOni.push_back(health);
		remainingTimesOni[health] = timeToFadeDurableHealthFeedback;
	}



	if (!obj) return;

	HealthLostInstantFeedback* fb = GET_SCRIPT(obj, HealthLostInstantFeedback);

	if (!fb) return;

	fb->Play();


}

void HUDController::UpdateCanvasHP(GameObject* targetCanvas, int health, bool darkened) {
	float4 magentaToSet = darkened ? colorMagentaDarkened : colorMagenta;
	float4 whiteToSet = darkened ? colorWhiteDarkened : colorWhite;
	int i = 0;
	for (GameObject* hpGameObject : targetCanvas->GetChildren()) {
		ComponentImage* hpComponent = hpGameObject->GetComponent<ComponentImage>();
		if (i < health) {
			hpComponent->SetColor(magentaToSet);
		} else {
			//If health is lower, set alpha to 0 so that effect may override if need be, but if not sets the health to "missing"
			hpComponent->SetColor(float4(whiteToSet.xyz(), 0.0f));
		}
		i++;
	}

	bool isFang = (targetCanvas->GetID() == fangHealthMainCanvas->GetID());

	if (isFang) {
		if (prevLivesFang > health) {
			OnHealthLost(targetCanvas, health);
			prevLivesFang = health;
		}
	} else {
		if (prevLivesOni > health) {
			OnHealthLost(targetCanvas, health);
			prevLivesOni = health;
		}
	}
}