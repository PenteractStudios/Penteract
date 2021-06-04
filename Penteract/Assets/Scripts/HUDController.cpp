#include "HUDController.h"

#include "GameObject.h"
#include "Components/UI/ComponentImage.h"
#include "HealthLostInstantFeedback.h"
#include "AbilityRefreshEffect.h"
#include "LowHPWarning.h"
#include "GameplaySystems.h"


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

void HUDController::Start() {
	//Resetting preLives and remainingTimes just in case
	prevLivesFang = prevLivesOni = MAX_HEALTH;
	for (int i = 0; i < MAX_HEALTH; i++) {
		remainingTimesFang[i] = remainingTimesOni[i] = 0;
	}

	for (int i = 0; i < 6; i++) {
		abilityCoolDownsRetreived[i] = true;
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

	if (fangHealthMainCanvas) {
		LoadHealthFeedbackStates(fangHealthMainCanvas, MAX_HEALTH);
	}

	for (int i = 0; i < static_cast<int>(Cooldowns::TOTAL); ++i) {
		cooldowns[i] = 0;
	}
}

void HUDController::Update() {
	if (!fangCanvas || !onimaruCanvas) return;
}

void HUDController::StopHealthLostInstantEffects(GameObject* targetCanvas) {
	for (int i = 0; i < MAX_HEALTH; i++) {
		GameObject* obj = targetCanvas->GetChildren()[i];
		if (!obj) return;
		HealthLostInstantFeedback* fb = GET_SCRIPT(obj, HealthLostInstantFeedback);
		if (!fb) return;
		fb->Stop();
	}
}

void HUDController::LoadHealthFeedbackStates(GameObject* targetCanvas, int health) {
	float4 magentaToSet = colorMagenta;
	float4 whiteToSet = colorWhite;
	int i = 0;
	for (GameObject* hpGameObject : targetCanvas->GetChildren()) {
		if (hpGameObject->GetChildren().size() > 0) {
			ComponentImage* hpComponent = hpGameObject->GetChildren()[1]->GetComponent<ComponentImage>();
			if (hpComponent) {
				if (i < health) {
					hpComponent->SetColor(magentaToSet);
				} else {
					//If health is lower, set alpha to 0 so that effect may override if need be, but if not sets the health to "missing"
					hpComponent->SetColor(float4(whiteToSet.xyz(), 0.0f));
				}
			}
		}
		i++;
	}
}

void HUDController::ChangePlayerHUD(int fangLives, int oniLives) {
	if (!fang || !onimaru) return;

	if (!fang->IsActive()) {
		fangCanvas->Disable();
		onimaruCanvas->Enable();
		StopHealthLostInstantEffects(onimaruHealthMainCanvas);
		LoadHealthFeedbackStates(onimaruHealthMainCanvas, oniLives);
	} else {
		onimaruCanvas->Disable();
		fangCanvas->Enable();
		StopHealthLostInstantEffects(fangHealthMainCanvas);
		LoadHealthFeedbackStates(fangHealthMainCanvas, fangLives);
	}

	remainingTimeActiveIndexesFang.clear();
	remainingTimeActiveIndexesOni.clear();
	for (int i = 0; i < MAX_HEALTH; i++) {
		remainingTimesFang[i] = 0;
		remainingTimesOni[i] = 0;
	}
	abilityCoolDownsRetreived[static_cast<int>(Cooldowns::SWITCH_SKILL)] = false;

	prevLivesFang = fangLives;
	prevLivesOni = oniLives;

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
	} else {
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
	} else if (onimaru->IsActive()) {
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

void HUDController::SetCooldownRetreival(Cooldowns cooldown) {
	abilityCoolDownsRetreived[static_cast<int>(cooldown)] = false;
}

float  HUDController::MapValue01(float value, float min, float max) {
	return (value - min) / (max - min);
}



void HUDController::UpdateCooldowns(float onimaruCooldown1, float onimaruCooldown2, float onimaruCooldown3, float fangCooldown1, float fangCooldown2, float fangCooldown3, float switchCooldown) {
	//The received cooldowns here range from 0 to 1
	cooldowns[static_cast<int>(Cooldowns::FANG_SKILL_1)] = fangCooldown1;
	cooldowns[static_cast<int>(Cooldowns::FANG_SKILL_2)] = fangCooldown2;
	cooldowns[static_cast<int>(Cooldowns::FANG_SKILL_3)] = fangCooldown3;
	cooldowns[static_cast<int>(Cooldowns::ONIMARU_SKILL_1)] = onimaruCooldown1;
	cooldowns[static_cast<int>(Cooldowns::ONIMARU_SKILL_2)] = onimaruCooldown2;
	cooldowns[static_cast<int>(Cooldowns::ONIMARU_SKILL_3)] = onimaruCooldown3;
	cooldowns[static_cast<int>(Cooldowns::SWITCH_SKILL)] = switchCooldown;

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

	bool activateEffect = currentHp <= LOW_HEALTH_WARNING ? true : false;

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

	float* remainingTimes = isFang ? remainingTimesFang : remainingTimesOni;

	if (indexes.size() > 0) {
		std::vector<std::vector<int>::iterator>vectorIndexesToRemove;

		for (std::vector<int>::iterator it = indexes.begin(); it != indexes.end(); ++it) {
			remainingTimes[(*it)] -= Time::GetDeltaTime();

			if (remainingTimes[(*it)] <= 0) {
				remainingTimes[(*it)] = 0;
				vectorIndexesToRemove.push_back(it);
			}

			float delta = remainingTimes[(*it)] / timeToFadeDurableHealthFeedback;


			ComponentImage* image = targetCanvas->GetChildren()[*it]->GetChildren()[1]->GetComponent<ComponentImage>();
			//We need access to image->SetAlphaTransparency to prevent possible errors
			if (delta < 0.5) {
				//Remapping
				delta = (delta - 0) / (0 - 0.5f);
				image->SetColor(float4(1.0f, 1.0f, 1.0f, delta));
			} else {
				image->SetColor(float4(1.0f, 1.0f, 1.0f, 1.0f));
			}
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
		UpdateFangCooldowns(fangSkillsMainCanvas, true);
		UpdateOnimaruCooldowns(onimaruSkillsSecondCanvas, false);
		UpdateCommonSkill();
	} else if (onimaru->IsActive()) {
		UpdateOnimaruCooldowns(onimaruSkillsMainCanvas, true);
		UpdateFangCooldowns(fangSkillsSecondCanvas, false);
		UpdateCommonSkill();
	}
}

void HUDController::UpdateCommonSkill() {
	std::vector<GameObject*> children = swapingSkillCanvas->GetChildren();
	if (children.size() > 2) {
		//Hardcoded value in hierarchy, changing the hierarchy without changing the index or the other way around will result in this not working properly
		ComponentImage* image = children[3]->GetComponent<ComponentImage>();
		if (image) {
			image->SetFillValue(cooldowns[static_cast<int>(Cooldowns::SWITCH_SKILL)]);
			AbilityCoolDownEffectCheck(static_cast<Cooldowns>(Cooldowns::SWITCH_SKILL), swapingSkillCanvas);
		}
	}
	//for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it) {
	//	ComponentImage* image = (*it)->GetComponent<ComponentImage>();
	//	if (image) {
	//		if (image->IsFill()) {
	//			image->SetFillValue(cooldowns[static_cast<int>(Cooldowns::SWITCH_SKILL)]);
	//			AbilityCoolDownEffectCheck(static_cast<Cooldowns>(Cooldowns::SWITCH_SKILL), swapingSkillCanvas);
	//		}
	//	}
	//}
}


void HUDController::UpdateFangCooldowns(GameObject* fangSkillCanvas, bool isMain) {
	std::vector<GameObject*> skills = fangSkillCanvas->GetChildren();
	int skill = static_cast<int>(Cooldowns::FANG_SKILL_1);
	for (std::vector<GameObject*>::iterator it = skills.begin(); it != skills.end(); ++it) {

		if ((*it)->GetChildren().size() > 0) {
			ComponentImage* image = (*it)->GetChildren()[2]->GetComponent<ComponentImage>();
			if (image) {
				if (image->IsFill()) {
					if (isMain) AbilityCoolDownEffectCheck(static_cast<Cooldowns>(skill), fangSkillCanvas);
					image->SetFillValue(cooldowns[skill]);
				}
			}
		}

		++skill;
	}
}



void HUDController::PlayCoolDownEffect(AbilityRefreshEffect* ef, Cooldowns cooldown) {
	if (ef != nullptr) {
		ef->Play();
		abilityCoolDownsRetreived[static_cast<int>(cooldown)] = true;
	}
}


void HUDController::AbilityCoolDownEffectCheck(Cooldowns cooldown, GameObject* canvas) {
	if (!abilityCoolDownsRetreived[static_cast<int>(cooldown)]) {
		if (canvas) {
			if (cooldowns[static_cast<int>(cooldown)] == 1.0f) {

				AbilityRefreshEffect* ef = nullptr;

				if (cooldown < Cooldowns::ONIMARU_SKILL_1) {
					if (canvas->GetChildren().size() > 0) {
						if (canvas->GetChildren()[static_cast<int>(cooldown)]->GetChildren().size() > 4) {
							ef = GET_SCRIPT(canvas->GetChildren()[static_cast<int>(cooldown)]->GetChildren()[0], AbilityRefreshEffect);
						}
					}
				} else if (cooldown < Cooldowns::SWITCH_SKILL) {
					if (canvas->GetChildren().size() > 0) {
						if (canvas->GetChildren()[static_cast<int>(cooldown) - 3]->GetChildren().size() > 4) {
							ef = GET_SCRIPT(canvas->GetChildren()[static_cast<int>(cooldown) - 3]->GetChildren()[0], AbilityRefreshEffect);
						}
					}
				} else {
					if (canvas->GetChildren().size() > 0) {
						ef = GET_SCRIPT(canvas->GetChildren()[2], AbilityRefreshEffect);
					}
				}


				PlayCoolDownEffect(ef, cooldown);
			}
		}
	}
}

void HUDController::UpdateOnimaruCooldowns(GameObject* onimaruSkillCanvas, bool isMain) {

	std::vector<GameObject*> skills = onimaruSkillCanvas->GetChildren();
	int skill = static_cast<int>(Cooldowns::ONIMARU_SKILL_1);
	for (std::vector<GameObject*>::iterator it = skills.begin(); it != skills.end(); ++it) {

		if ((*it)->GetChildren().size() > 0) {
			ComponentImage* image = (*it)->GetChildren()[2]->GetComponent<ComponentImage>();
			if (image) {
				if (image->IsFill()) {
					if (isMain) AbilityCoolDownEffectCheck(static_cast<Cooldowns>(skill), onimaruSkillCanvas);
					image->SetFillValue(cooldowns[skill]);
				}
			}
		}

		++skill;
	}

	/*std::vector<GameObject*> children = onimaruSkillCanvas->GetChildren();
	int skill = static_cast<int>(Cooldowns::ONIMARU_SKILL_1);
	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it) {
		std::vector<GameObject*> skills = (*it)->GetChildren();
		for (std::vector<GameObject*>::iterator itSkills = skills.begin(); itSkills != skills.end(); ++itSkills) {
			ComponentImage* image = (*itSkills)->GetComponent<ComponentImage>();
			if (image) {
				if (image->IsFill()) {
					switch (skill) {
						case static_cast<int>(Cooldowns::ONIMARU_SKILL_1) : {
							if (isMain) AbilityCoolDownEffectCheck(Cooldowns::ONIMARU_SKILL_1, onimaruSkillCanvas);
							image->SetFillValue(cooldowns[static_cast<int>(Cooldowns::ONIMARU_SKILL_1)]);
							break;
						}

						case static_cast<int>(Cooldowns::ONIMARU_SKILL_2) : {
							if (isMain) AbilityCoolDownEffectCheck(Cooldowns::ONIMARU_SKILL_2, onimaruSkillCanvas);
							image->SetFillValue(cooldowns[static_cast<int>(Cooldowns::ONIMARU_SKILL_2)]);
							break;
						}
						case static_cast<int>(Cooldowns::ONIMARU_SKILL_3) : {
							if (isMain) AbilityCoolDownEffectCheck(Cooldowns::ONIMARU_SKILL_3, onimaruSkillCanvas);
							image->SetFillValue(cooldowns[static_cast<int>(Cooldowns::ONIMARU_SKILL_3)]);
							break;
						}
						default:
							break;
					}
				}
			}
		}
		++skill;
	}*/
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