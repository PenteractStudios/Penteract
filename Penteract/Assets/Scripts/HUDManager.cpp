#include "HUDManager.h"
#include "PlayerController.h";
#include "Components/UI/ComponentTransform2D.h"
#include "Components/UI/ComponentImage.h"
#include "AbilityRefeshFX.h"

#define HIERARCHY_INDEX_ABILITY_FILL 1
#define HIERARCHY_INDEX_ABILITY_DURATION_FILL 2
#define HIERARCHY_INDEX_ABILITY_EFFECT 3
#define HIERARCHY_INDEX_ABILITY_PICTO_SHADE 5
#define HIERARCHY_INDEX_ABILITY_KEY_FILL 6

#define HIERARCHY_INDEX_SWITCH_ABILITY_FILL 1
#define HIERARCHY_INDEX_SWITCH_ABILITY_DURATION_FILL 2
#define HIERARCHY_INDEX_SWITCH_ABILITY_IN_USE_WHITE 3
#define HIERARCHY_INDEX_SWITCH_ABILITY_IN_USE_GLOW 4
#define HIERARCHY_INDEX_SWITCH_ABILITY_EFFECT 5
#define HIERARCHY_INDEX_SWITCH_ABILITY_PICTO_SHADE 7
#define HIERARCHY_INDEX_SWITCH_ABILITY_KEY_FILL 8

#define HIERARCHY_INDEX_HEALTH_BACKGROUND 0
#define HIERARCHY_INDEX_HEALTH_LOST_FEEDBACK 1
#define HIERARCHY_INDEX_HEALTH_FILL 2
#define HIERARCHY_INDEX_HEALTH_OVERLAY 3
#define HIERARCHY_INDEX_HEALTH_TEXT 4

#define HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_UP 0
#define HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_DOWN 1
#define HIERARCHY_INDEX_SWITCH_HEALTH_FILL 2

EXPOSE_MEMBERS(HUDManager) {
	MEMBER(MemberType::GAME_OBJECT_UID, playerObjectUID),
		MEMBER(MemberType::GAME_OBJECT_UID, fangSkillParentUID),
		MEMBER(MemberType::GAME_OBJECT_UID, onimaruSkillParentUID),
		MEMBER(MemberType::GAME_OBJECT_UID, switchSkillParentUID),
		MEMBER(MemberType::GAME_OBJECT_UID, fangHealthParentUID),
		MEMBER(MemberType::GAME_OBJECT_UID, onimaruHealthParentUID),
		MEMBER(MemberType::GAME_OBJECT_UID, switchHealthParentUID)
};

GENERATE_BODY_IMPL(HUDManager);

void HUDManager::Start() {
	GameObject* playerControllerObj = GameplaySystems::GetGameObject(playerObjectUID);
	if (playerControllerObj) {
		playerController = GET_SCRIPT(playerControllerObj, PlayerController);

		onimaruObj = playerControllerObj->GetChild("Onimaru");
		fangObj = playerControllerObj->GetChild("Fang");
	}
	fangSkillParent = GameplaySystems::GetGameObject(fangSkillParentUID);
	onimaruSkillParent = GameplaySystems::GetGameObject(onimaruSkillParentUID);
	switchSkillParent = GameplaySystems::GetGameObject(switchSkillParentUID);

	if (fangSkillParent && onimaruSkillParent && switchSkillParent) {
		skillsFang = fangSkillParent->GetChildren();
		skillsOni = onimaruSkillParent->GetChildren();

		//Vector used later to avoid a flicker on first swtich
		std::vector<ComponentTransform2D*>oniTransforms;

		for (int i = 0; i < static_cast<int>(Cooldowns::TOTAL); i++) {
			ComponentTransform2D* transform2D = nullptr;
			if (i < static_cast<int>(Cooldowns::ONIMARU_SKILL_1)) {
				//Fang skill
				transform2D = skillsFang[i]->GetComponent<ComponentTransform2D>();

			} else if (i != static_cast<int>(Cooldowns::SWITCH_SKILL)) {
				//Onimaru skill
				transform2D = skillsOni[i - 3]->GetComponent<ComponentTransform2D>();
				if (transform2D) {
					oniTransforms.push_back(transform2D);
				}
			} else {
				transform2D = switchSkillParent->GetComponent<ComponentTransform2D>();
			}

			if (transform2D) {
				cooldownTransformOriginalPositions[i] = transform2D->GetPosition();
			}
		}

		//Set onimaru abilities position to the collapsed position so as to avoid a flicker on first swtich
		for (int i = 0; i < oniTransforms.size(); i++) {
			oniTransforms[i]->SetPosition(cooldownTransformOriginalPositions[static_cast<int>(Cooldowns::SWITCH_SKILL)]);
		}

		GameObject* pictoShadeObj = switchSkillParent->GetChild("PictoShade");

		if (pictoShadeObj) {
			switchShadeTransform = pictoShadeObj->GetComponent<ComponentTransform2D>();
		}


		if (switchSkillParent) {
			std::vector<GameObject*> switchChildren = switchSkillParent->GetChildren();
			if (switchChildren.size() > HIERARCHY_INDEX_SWITCH_ABILITY_KEY_FILL - 1) {
				switchChildren[HIERARCHY_INDEX_SWITCH_ABILITY_IN_USE_WHITE]->Disable();
				switchChildren[HIERARCHY_INDEX_SWITCH_ABILITY_IN_USE_GLOW]->Enable();
				switchGlowImage = switchChildren[HIERARCHY_INDEX_SWITCH_ABILITY_IN_USE_GLOW]->GetComponent<ComponentImage>();
				if (switchGlowImage) {
					switchGlowImage->SetColor(float4(1.0f, 1.0f, 1.0f, 0.0f));
				}
			}
		}

	}

	fangHealthParent = GameplaySystems::GetGameObject(fangHealthParentUID);
	onimaruHealthParent = GameplaySystems::GetGameObject(onimaruHealthParentUID);
	switchHealthParent = GameplaySystems::GetGameObject(switchHealthParentUID);

	if (fangHealthParent && onimaruHealthParent) {
		ComponentTransform2D* pos = nullptr;
		pos = fangHealthParent->GetComponent<ComponentTransform2D>();
		if (pos) originalFangHealthPosition = pos->GetPosition();
		pos = nullptr;
		pos = onimaruHealthParent->GetComponent<ComponentTransform2D>();
		if (pos) originalOnimaruHealthPosition = pos->GetPosition();
		fangHealthChildren = fangHealthParent->GetChildren();
		onimaruHealthChildren = onimaruHealthParent->GetChildren();

		InitializeHealth();
	}

	if (switchHealthParent) {
		switchHealthChildren = switchHealthParent->GetChildren();

		if (switchHealthChildren.size() == 3) {
			ComponentTransform2D* transform = switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_UP]->GetComponent<ComponentTransform2D>();
			if (transform) originalStrokeSize = transform->GetSize();
			transform = switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_FILL]->GetComponent<ComponentTransform2D>();
			if (transform) originalFillPos = transform->GetPosition();
		}
	}

}

void HUDManager::Update() {
	ManageSwitch();
}

void HUDManager::UpdateCooldowns(float onimaruCooldown1, float onimaruCooldown2, float onimaruCooldown3, float fangCooldown1, float fangCooldown2, float fangCooldown3, float switchCooldown, float fangUltimateRemainingNormalizedValue, float oniUltimateRemainingNormalizedValue) {
	cooldowns[static_cast<int>(Cooldowns::FANG_SKILL_1)] = fangCooldown1;
	cooldowns[static_cast<int>(Cooldowns::FANG_SKILL_2)] = fangCooldown2;
	cooldowns[static_cast<int>(Cooldowns::FANG_SKILL_3)] = fangUltimateRemainingNormalizedValue == 0 ? fangCooldown3 : 1.0f;
	cooldowns[static_cast<int>(Cooldowns::ONIMARU_SKILL_1)] = onimaruCooldown1;
	cooldowns[static_cast<int>(Cooldowns::ONIMARU_SKILL_2)] = onimaruCooldown2;
	cooldowns[static_cast<int>(Cooldowns::ONIMARU_SKILL_3)] = oniUltimateRemainingNormalizedValue == 0 ? onimaruCooldown3 : 1.0f;
	cooldowns[static_cast<int>(Cooldowns::SWITCH_SKILL)] = switchCooldown;

	if (onimaruObj && fangObj && fangSkillParent && onimaruSkillParent) {

		if (fangObj->IsActive()) {
			UpdateVisualCooldowns(fangSkillParent, static_cast<int>(Cooldowns::FANG_SKILL_1));
			SetRemainingDurationNormalizedValue(fangSkillParent, static_cast<int>(Cooldowns::FANG_SKILL_3), fangUltimateRemainingNormalizedValue);
		} else {
			UpdateVisualCooldowns(onimaruSkillParent, static_cast<int>(Cooldowns::ONIMARU_SKILL_1));
			SetRemainingDurationNormalizedValue(onimaruSkillParent, static_cast<int>(Cooldowns::ONIMARU_SKILL_3) - 3, oniUltimateRemainingNormalizedValue);
		}
	}
	UpdateCommonSkillVisualCooldown();
}

void HUDManager::UpdateHealth(float fangHealth, float onimaruHealth) {
	if (!fangObj || !onimaruObj || !playerController) return;
	if (fangHealthChildren.size() != 5 || onimaruHealthChildren.size() != 5) return;

	float health = fangObj->IsActive() ? fangHealth : onimaruHealth;
	float maxHealth = fangObj->IsActive() ? playerController->GetFangMaxHealth() : playerController->GetOnimaruMaxHealth();

	ComponentImage* healthFill = nullptr;
	healthFill = fangObj->IsActive() ? fangHealthChildren[HIERARCHY_INDEX_HEALTH_FILL]->GetComponent<ComponentImage>() : onimaruHealthChildren[HIERARCHY_INDEX_HEALTH_FILL]->GetComponent<ComponentImage>();
	if (healthFill) {
		if (healthFill->IsFill()) {
			healthFill->SetFillValue(health / maxHealth);
		}
	}

	ComponentText* healthText = nullptr;
	healthText = fangObj->IsActive() ? fangHealthChildren[HIERARCHY_INDEX_HEALTH_TEXT]->GetComponent<ComponentText>() : onimaruHealthChildren[HIERARCHY_INDEX_HEALTH_TEXT]->GetComponent<ComponentText>();
	if (healthText) {
		healthText->SetText(std::to_string((int)health));
	}

}

void HUDManager::HealthRegeneration(float health) {
	if (!fangObj || !onimaruObj || !playerController) return;
	if (fangHealthChildren.size() != 5 || onimaruHealthChildren.size() != 5) return;

	float maxHealth = fangObj->IsActive() ? playerController->GetOnimaruMaxHealth() : playerController->GetFangMaxHealth();

	ComponentImage* healthFill = nullptr;
	healthFill = fangObj->IsActive() ? onimaruHealthChildren[HIERARCHY_INDEX_HEALTH_FILL]->GetComponent<ComponentImage>() : fangHealthChildren[HIERARCHY_INDEX_HEALTH_FILL]->GetComponent<ComponentImage>();
	if (healthFill) {
		if (healthFill->IsFill()) {
			healthFill->SetFillValue(health / maxHealth);
		}
	}

	ComponentText* healthText = nullptr;
	healthText = fangObj->IsActive() ? onimaruHealthChildren[HIERARCHY_INDEX_HEALTH_TEXT]->GetComponent<ComponentText>() : fangHealthChildren[HIERARCHY_INDEX_HEALTH_TEXT]->GetComponent<ComponentText>();
	if (healthText) {
		healthText->SetText(std::to_string((int)health));
	}
}

void HUDManager::StartCharacterSwitch() {
	//TODO initialization
	switchTimer = 0;
	switchState = SwitchState::PRE_COLLAPSE;
	if (switchSkillParent) {
		std::vector<GameObject*> switchChildren = switchSkillParent->GetChildren();
		if (switchChildren.size() > HIERARCHY_INDEX_SWITCH_ABILITY_KEY_FILL - 1) {
			switchChildren[HIERARCHY_INDEX_SWITCH_ABILITY_IN_USE_WHITE]->Enable();

			switchChildren[HIERARCHY_INDEX_SWITCH_ABILITY_IN_USE_GLOW]->Enable();

			if (switchGlowImage) {
				switchGlowImage->SetColor(float4(1.0f, 1.0f, 1.0f, 1.0f));
			}
		}
	}
}

void HUDManager::SetCooldownRetreival(Cooldowns cooldown) {
	abilityCoolDownsRetreived[static_cast<int>(cooldown)] = false;
	if (cooldown != Cooldowns::SWITCH_SKILL)
		SetPictoState(cooldown, PictoState::UNAVAILABLE);
}

void HUDManager::StartUsingSkill(Cooldowns cooldown) {
	SetPictoState(cooldown, PictoState::IN_USE);
}

void HUDManager::StopUsingSkill(Cooldowns cooldown) {
	if (cooldown != Cooldowns::SWITCH_SKILL)
		SetPictoState(cooldown, cooldowns[static_cast<int>(cooldown)] < 1.0f ? PictoState::UNAVAILABLE : PictoState::AVAILABLE);
}

void HUDManager::UpdateVisualCooldowns(GameObject* canvas, int startingIt) {

	std::vector<GameObject*> skills = canvas->GetChildren();
	//Skills size is supposed to be 3, for each character has 3 skills
	if (skills.size() == 3) {

		int skill = startingIt;
		for (std::vector<GameObject*>::iterator it = skills.begin(); it != skills.end(); ++it) {

			ComponentImage* fillImage = nullptr;
			ComponentImage* pictogramImage = nullptr;
			ComponentText* text = nullptr;
			ComponentImage* textFill = nullptr;
			fillImage = (*it)->GetChildren()[HIERARCHY_INDEX_ABILITY_FILL]->GetComponent<ComponentImage>();
			pictogramImage = (*it)->GetChildren()[HIERARCHY_INDEX_ABILITY_PICTO_SHADE]->GetComponent<ComponentImage>();

			textFill = (*it)->GetChildren()[HIERARCHY_INDEX_ABILITY_KEY_FILL]->GetComponent<ComponentImage>();
			if (fillImage && pictogramImage) {

				if (cooldowns[skill] < 1) {
					//On Cooldown
					fillImage->SetColor(float4(skillColorNotAvailable.xyz(), 0.3f + cooldowns[skill]));
				} else {
					//Available
					fillImage->SetColor(skillColorAvailable);
				}

				if (fillImage->IsFill()) {
					fillImage->SetFillValue(cooldowns[skill]);
				}
			}

			GameObject* textParent = (*it)->GetChildren()[HIERARCHY_INDEX_ABILITY_KEY_FILL];

			if (textParent) {
				text = textParent->GetChild(1)->GetComponent<ComponentText>();
				if (text) {
					text->SetFontColor(cooldowns[skill] < 1 ? buttonTextColorNotAvailable : buttonTextColorAvailable);
				}
			}

			if (textFill) {
				textFill->SetColor(cooldowns[skill] < 1 ? buttonColorNotAvailable : buttonColorAvailable);
			}

			AbilityCoolDownEffectCheck(static_cast<Cooldowns>(skill), canvas);

			++skill;
		}
	}
}


void HUDManager::SetRemainingDurationNormalizedValue(GameObject* canvas, int index, float normalizedValue) {
	if (!canvas)return;
	std::vector<GameObject*> children = canvas->GetChildren();
	if (children.size() <= index)return;

	GameObject* fillHolder = children[index]->GetChild(HIERARCHY_INDEX_ABILITY_DURATION_FILL);
	if (fillHolder) {
		ComponentImage* fill = fillHolder->GetComponent<ComponentImage>();
		if (fill) {
			fill->SetFillValue(normalizedValue);
		}
	}
}


//Hierarchy sensitive method
void HUDManager::AbilityCoolDownEffectCheck(Cooldowns cooldown, GameObject* canvas) {

	if (!abilityCoolDownsRetreived[static_cast<int>(cooldown)]) {
		if (cooldowns[static_cast<int>(cooldown)] == 1.0f) {
			if (canvas) {
				AbilityRefeshFX* ef = nullptr;
				//AbilityRefreshEffectProgressBar* pef = nullptr;

				if (cooldown < Cooldowns::ONIMARU_SKILL_1) {
					//Fang skill
					if (canvas->GetChildren().size() > 0) {
						//if (canvas->GetChildren()[static_cast<int>(cooldown)]->GetChildren().size() > HIERARCHY_INDEX_MAIN_BUTTON_UP) {
						ef = GET_SCRIPT(canvas->GetChildren()[static_cast<int>(cooldown)]->GetChildren()[HIERARCHY_INDEX_ABILITY_EFFECT], AbilityRefeshFX);
						//}
					}
				} else if (cooldown < Cooldowns::SWITCH_SKILL) {
					//Onimaru skill
					if (canvas->GetChildren().size() > 0) {
						//if (canvas->GetChildren()[static_cast<int>(cooldown) - 3]->GetChildren().size() > HIERARCHY_INDEX_MAIN_BUTTON_UP) {
						ef = GET_SCRIPT(canvas->GetChild(static_cast<int>(cooldown) - 3)->GetChildren()[HIERARCHY_INDEX_ABILITY_EFFECT], AbilityRefeshFX);

						//}
					}
				} else {
					if (canvas->GetChildren().size() > 0) {
						ef = GET_SCRIPT(canvas->GetChild(HIERARCHY_INDEX_SWITCH_ABILITY_EFFECT), AbilityRefeshFX);
						//pef = GET_SCRIPT(canvas->GetChild(HIERARCHY_INDEX_SWAP_ABILITY_EFFECT), AbilityRefreshEffectProgressBar);


					}
				}

				if (ef) {
					PlayCoolDownEffect(ef, cooldown);
					//PlayProgressBarEffect(pef, cooldown);
					abilityCoolDownsRetreived[static_cast<int>(cooldown)] = true;
					SetPictoState(cooldown, PictoState::AVAILABLE);
				}
			} else {
				//Canvas was passed as nullptr, which means that no effect must be played, only that the bool must be set
				abilityCoolDownsRetreived[static_cast<int>(cooldown)] = true;
				SetPictoState(cooldown, PictoState::AVAILABLE);
			}
		}
	}
}

void HUDManager::UpdateCommonSkillVisualCooldown() {
	if (!switchSkillParent) return;


	std::vector<GameObject*> children = switchSkillParent->GetChildren();

	//TODO Check for hierarchy size

	ComponentImage* fillColor = children[HIERARCHY_INDEX_SWITCH_ABILITY_FILL]->GetComponent<ComponentImage>();
	ComponentImage* image = children[HIERARCHY_INDEX_SWITCH_ABILITY_PICTO_SHADE]->GetComponent<ComponentImage>();

	GameObject* textParent = children[HIERARCHY_INDEX_SWITCH_ABILITY_KEY_FILL];
	ComponentText* text = nullptr;

	if (textParent) {
		if (textParent->GetChildren().size() > 1)
			text = textParent->GetChild(1)->GetComponent<ComponentText>();
	}

	ComponentImage* textFill = children[HIERARCHY_INDEX_SWITCH_ABILITY_KEY_FILL]->GetComponent<ComponentImage>();

	if (fillColor && image) {
		fillColor->SetFillValue(cooldowns[static_cast<int>(Cooldowns::SWITCH_SKILL)]);
		fillColor->SetColor(cooldowns[static_cast<int>(Cooldowns::SWITCH_SKILL)] < 1 ? float4(switchSkillColorNotAvailable.xyz(), 0.3f + cooldowns[static_cast<int>(Cooldowns::SWITCH_SKILL)]) : switchSkillColorAvailable);
	}

	AbilityCoolDownEffectCheck(Cooldowns::SWITCH_SKILL, switchSkillParent);

	if (text) {
		text->SetFontColor(cooldowns[static_cast<int>(Cooldowns::SWITCH_SKILL)] < 1 ? buttonTextColorNotAvailable : buttonTextColorAvailable);
	}

	if (textFill) {
		textFill->SetColor(cooldowns[static_cast<int>(Cooldowns::SWITCH_SKILL)] < 1 ? buttonColorNotAvailable : buttonColorAvailable);
	}


}

void HUDManager::ManageSwitch() {
	if (!fangSkillParent || !onimaruSkillParent || !fangHealthParent || !onimaruHealthParent || !switchHealthParent || !fangObj || !onimaruObj) return;
	if (skillsFang.size() != 3 || skillsOni.size() != 3) return;
	if (fangHealthChildren.size() != 5 || onimaruHealthChildren.size() != 5) return;

	ComponentTransform2D* transform2D = nullptr;
	ComponentTransform2D* health = nullptr;
	ComponentImage* backgroundImage = nullptr;
	ComponentImage* fillImage = nullptr;
	ComponentImage* overlayImage = nullptr;
	ComponentText* healthText = nullptr;
	ComponentImage* switchBarFillImage = nullptr;
	ComponentImage* switchHealthStroke = nullptr;
	ComponentTransform2D* switchHealthStrokeTransform2D = nullptr;
	ComponentTransform2D* switchHealthFillTransform2D = nullptr;

	switch (switchState) {
	case SwitchState::IDLE:

		//TODO switch icon rotates over time, reset timer when rotation reaches 360º
		//REQUIRED TO expose rotation on Tesseract
		/*if (switchShadeTransform) {
			Quat rotToAdd;
			rotToAdd.SetFromAxisAngle(float4(0, 0, 1, 1), 1 / switchSymbolRotationTime),
			switchShadeTransform->SetRotation(rotToAdd * switchShadeTransform->GetGlobalRotation().ToQuat());
		}*/

		//This code handles the color grading progressively increasing and decreasing alpha
		if (switchSkillParent) {
			std::vector<GameObject*> children = switchSkillParent->GetChildren();
			ComponentImage* fillImage = children[HIERARCHY_INDEX_SWITCH_ABILITY_FILL]->GetComponent<ComponentImage>();
			if (fillImage) {

				float delta = switchColorTimer / switchColorTotalTime;

				if (switchColorIncreasing) {
					fillImage->SetColor(float4(fillImage->GetColor().xyz(), Lerp(0.3f, 1, delta)));
				} else {
					fillImage->SetColor(float4(fillImage->GetColor().xyz(), Lerp(1, 0.3f, delta)));
				}
				switchColorTimer += Time::GetDeltaTime();
			}
		}

		//Reset color timer and invert the toggle for increasing/decreasing
		if (switchColorTimer >= switchColorTotalTime) {
			switchColorTimer = 0;
			switchColorIncreasing = !switchColorIncreasing;
		}

		break;
	case SwitchState::PRE_COLLAPSE:

		//TODO make glow happen
		//TODO activate white effect under picto 

		if (switchTimer > switchPreCollapseMovementTime) {
			switchTimer = switchPreCollapseMovementTime;
		}

		if (switchGlowImage) {
			switchGlowImage->SetColor(float4(1.0f, 1.0f, 1.0f, 1 - Clamp(switchTimer / switchPreCollapseMovementTime, 0.0f, 0.7f)));
		}

		if (fangSkillParent->IsActive()) {
			for (int i = 0; i < skillsFang.size(); ++i) {
				transform2D = skillsFang[i]->GetComponent<ComponentTransform2D>();
				if (transform2D) {
					transform2D->SetPosition(float3::Lerp(cooldownTransformOriginalPositions[i], cooldownTransformOriginalPositions[i] + float3(switchExtraOffset + i * 10.0f, 0, 0), switchTimer / switchPreCollapseMovementTime));
				}

			}
		} else {
			for (int i = 0; i < skillsOni.size(); ++i) {
				transform2D = skillsOni[i]->GetComponent<ComponentTransform2D>();
				if (transform2D) {
					transform2D->SetPosition(float3::Lerp(cooldownTransformOriginalPositions[i + 3], cooldownTransformOriginalPositions[i + 3] + float3(switchExtraOffset + i * 10.0f, 0, 0), switchTimer / switchPreCollapseMovementTime));
				}
			}
		}

		if (switchTimer == switchPreCollapseMovementTime) {
			if (switchGlowImage) {
				switchGlowImage->SetColor(float4(1.0f, 1.0f, 1.0f, 0));
			}
			switchState = SwitchState::COLLAPSE;
			switchTimer = 0;
		}

		break;
	case SwitchState::COLLAPSE:
		SetCooldownRetreival(Cooldowns::SWITCH_SKILL);

		if (switchTimer > switchCollapseMovementTime) {
			switchTimer = switchCollapseMovementTime;
		}

		if (switchGlowImage) {
			switchGlowImage->SetColor(float4(1.0f, 1.0f, 1.0f, 1 - Clamp01(0.7f + switchTimer / switchPreCollapseMovementTime)));
		}

		if (fangSkillParent->IsActive()) {
			for (int i = 0; i < skillsFang.size(); ++i) {
				transform2D = skillsFang[i]->GetComponent<ComponentTransform2D>();
				if (transform2D) {
					transform2D->SetPosition(float3::Lerp(cooldownTransformOriginalPositions[i] + float3(switchExtraOffset + i * 10.0f, 0, 0), cooldownTransformOriginalPositions[static_cast<int>(Cooldowns::SWITCH_SKILL)], switchTimer / switchCollapseMovementTime));
				}

			}
		} else {
			for (int i = 0; i < skillsOni.size(); ++i) {
				transform2D = skillsOni[i]->GetComponent<ComponentTransform2D>();
				if (transform2D) {
					transform2D->SetPosition(float3::Lerp(cooldownTransformOriginalPositions[i + 3] + float3(switchExtraOffset + i * 10.0f, 0, 0), cooldownTransformOriginalPositions[static_cast<int>(Cooldowns::SWITCH_SKILL)], switchTimer / switchCollapseMovementTime));
				}
			}
		}

		//Health handling

		if (fangObj->IsActive()) {
			health = onimaruHealthParent->GetComponent<ComponentTransform2D>();
			backgroundImage = onimaruHealthChildren[HIERARCHY_INDEX_HEALTH_BACKGROUND]->GetComponent<ComponentImage>();
			fillImage = onimaruHealthChildren[HIERARCHY_INDEX_HEALTH_FILL]->GetComponent<ComponentImage>();
			overlayImage = onimaruHealthChildren[HIERARCHY_INDEX_HEALTH_OVERLAY]->GetComponent<ComponentImage>();
			healthText = onimaruHealthChildren[HIERARCHY_INDEX_HEALTH_TEXT]->GetComponent<ComponentText>();
			switchHealthStroke = switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_DOWN]->GetComponent<ComponentImage>();

			if (health) {
				health->SetPosition(float3::Lerp(originalOnimaruHealthPosition + float3(healthOffset, 0, 0), originalOnimaruHealthPosition, switchTimer / switchCollapseMovementTime));
			}
		}
		else {
			health = fangHealthParent->GetComponent<ComponentTransform2D>();
			backgroundImage = fangHealthChildren[HIERARCHY_INDEX_HEALTH_BACKGROUND]->GetComponent<ComponentImage>();
			fillImage = fangHealthChildren[HIERARCHY_INDEX_HEALTH_FILL]->GetComponent<ComponentImage>();
			overlayImage = fangHealthChildren[HIERARCHY_INDEX_HEALTH_OVERLAY]->GetComponent<ComponentImage>();
			healthText = fangHealthChildren[HIERARCHY_INDEX_HEALTH_TEXT]->GetComponent<ComponentText>();
			switchHealthStroke = switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_UP]->GetComponent<ComponentImage>();

			if (health) {
				health->SetPosition(float3::Lerp(originalFangHealthPosition, originalFangHealthPosition - float3(healthOffset, 0, 0), switchTimer / switchCollapseMovementTime));
			}
		}


		if (backgroundImage) {
			backgroundImage->SetColor(float4::Lerp(healthBarBackgroundColor, healthBarBackgroundColorInBackground, switchTimer / switchCollapseMovementTime));
		}

		if (fillImage) {
			fillImage->SetColor(float4::Lerp(healthFillBarColor, healthFillBarColorInBackground, switchTimer / switchCollapseMovementTime));
		}

		if (overlayImage) {
			overlayImage->SetColor(float4::Lerp(healthOverlayColor, healthOverlayColorInBackground, switchTimer / switchCollapseMovementTime));
		}

		if (healthText) {
			healthText->SetFontColor(float4::Lerp(healthTextColor, healthTextColorInBackground, switchTimer / switchCollapseMovementTime));
		}

		if (switchHealthStroke) {
			switchHealthStroke->SetColor(float4::Lerp(healthSwitchStrokeInitialColor, healthSwitchStrokeChangingColor, switchTimer / switchCollapseMovementTime));
		}

		switchHealthFillTransform2D = switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_FILL]->GetComponent<ComponentTransform2D>();
		if (switchHealthFillTransform2D) {
			switchHealthFillTransform2D->SetScale(float3::Lerp(float3(1, 1, 1), float3(0, 1, 1), switchTimer / switchCollapseMovementTime));
		}

		if (switchTimer == switchCollapseMovementTime) {
			switchState = SwitchState::PRE_DEPLOY;

			if (fangSkillParent->IsActive()) {
				fangSkillParent->Disable();
				onimaruSkillParent->Enable();
			} else {
				fangSkillParent->Enable();
				onimaruSkillParent->Disable();
			}
			if (switchHealthFillTransform2D) {
				switchHealthStrokeTransform2D = fangObj->IsActive() ? switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_UP]->GetComponent<ComponentTransform2D>() : switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_DOWN]->GetComponent<ComponentTransform2D>();
				if (switchHealthStrokeTransform2D) {
					if (!fangObj->IsActive()) switchHealthFillTransform2D->SetPosition(switchHealthStrokeTransform2D->GetPosition());
					else switchHealthFillTransform2D->SetPosition(originalFillPos);
				}
			}
			switchTimer = 0;
		}

		break;
	case SwitchState::PRE_DEPLOY:
		// Workaround until we have SetPivot

		if (switchTimer > switchBarGrowShrinkTime) {
			switchTimer = switchBarGrowShrinkTime;
		}
		if (switchHealthStrokeGrowing) {
			float delta = switchTimer / switchBarGrowShrinkTime;
			float scale = Lerp(1, 2, delta);
			switchHealthStrokeTransform2D = fangObj->IsActive() ? switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_DOWN]->GetComponent<ComponentTransform2D>() : switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_UP]->GetComponent<ComponentTransform2D>();
			if (switchHealthStrokeTransform2D) {
				switchHealthStrokeTransform2D->SetScale(float3(1, scale, 1));
			}
		} else if (switchHealthStrokeShrinking) {
			float delta = switchTimer / switchBarGrowShrinkTime;
			float scale = Lerp(2, 1, delta);
			switchHealthStrokeTransform2D = fangObj->IsActive() ? switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_UP]->GetComponent<ComponentTransform2D>() : switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_DOWN]->GetComponent<ComponentTransform2D>();
			if (switchHealthStrokeTransform2D) {
				switchHealthStrokeTransform2D->SetScale(float3(1, scale, 1));
			}
		}
		if (switchTimer == switchBarGrowShrinkTime) {
			if (switchHealthStrokeGrowing) {
				switchHealthStrokeGrowing = false;
				switchHealthStrokeShrinking = true;
				ComponentTransform2D* transform = nullptr;

				switchHealthStroke = fangObj->IsActive() ? switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_UP]->GetComponent<ComponentImage>() : switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_DOWN]->GetComponent<ComponentImage>();
				if (fangObj->IsActive()) {
					switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_UP]->Enable();
					transform = switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_UP]->GetComponent<ComponentTransform2D>();
					if (transform) transform->SetScale(float3(1, 2, 1));
					if (switchHealthStroke) switchHealthStroke->SetColor(healthSwitchStrokeChangingColor);
					transform = switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_DOWN]->GetComponent<ComponentTransform2D>();
					if (transform) transform->SetScale(float3(1, 1, 1));
					switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_DOWN]->Disable();
				} else {
					switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_DOWN]->Enable();
					transform = switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_DOWN]->GetComponent<ComponentTransform2D>();
					if (transform) transform->SetScale(float3(1, 2, 1));
					if (switchHealthStroke) switchHealthStroke->SetColor(healthSwitchStrokeChangingColor);
					transform = switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_UP]->GetComponent<ComponentTransform2D>();
					if (transform) transform->SetScale(float3(1, 1, 1));
					switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_UP]->Disable();
				}
				//switchHealthStrokeTransform2D->SetPivot(float2(0, 0)); Need to expose function in engine

			} else if (switchHealthStrokeShrinking) {
				switchHealthStrokeGrowing = true;
				switchHealthStrokeShrinking = false;
				//switchHealthStrokeTransform2D->SetPivot(float2(1, 1));
				switchState = SwitchState::DEPLOY;
			}
			switchTimer = 0;
		}
		break;
	case SwitchState::DEPLOY:

		if (switchTimer > switchDeployMovementTime) {
			switchTimer = switchDeployMovementTime;
		}

		if (fangSkillParent->IsActive()) {
			for (int i = 0; i < skillsFang.size(); ++i) {
				transform2D = skillsFang[i]->GetComponent<ComponentTransform2D>();
				if (transform2D) {
					transform2D->SetPosition(float3::Lerp(cooldownTransformOriginalPositions[static_cast<int>(Cooldowns::SWITCH_SKILL)], cooldownTransformOriginalPositions[i] + float3(switchExtraOffset + i * 10.0f, 0, 0), switchTimer / switchCollapseMovementTime));
				}

			}
		} else {
			for (int i = 0; i < skillsOni.size(); ++i) {
				transform2D = skillsOni[i]->GetComponent<ComponentTransform2D>();
				if (transform2D) {
					transform2D->SetPosition(float3::Lerp(cooldownTransformOriginalPositions[static_cast<int>(Cooldowns::SWITCH_SKILL)], cooldownTransformOriginalPositions[i + 3] + float3(switchExtraOffset + i * 10.0f, 0, 0), switchTimer / switchCollapseMovementTime));
				}
			}
		}

		if (fangObj->IsActive()) {
			health = fangHealthParent->GetComponent<ComponentTransform2D>();
			backgroundImage = fangHealthChildren[HIERARCHY_INDEX_HEALTH_BACKGROUND]->GetComponent<ComponentImage>();
			fillImage = fangHealthChildren[HIERARCHY_INDEX_HEALTH_FILL]->GetComponent<ComponentImage>();
			overlayImage = fangHealthChildren[HIERARCHY_INDEX_HEALTH_OVERLAY]->GetComponent<ComponentImage>();
			healthText = fangHealthChildren[HIERARCHY_INDEX_HEALTH_TEXT]->GetComponent<ComponentText>();
			switchHealthStroke = switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_UP]->GetComponent<ComponentImage>();

			if (health) {
				health->SetPosition(float3::Lerp(originalFangHealthPosition - float3(healthOffset, 0, 0), originalFangHealthPosition, switchTimer / switchCollapseMovementTime));
			}
		}
		else {
			health = onimaruHealthParent->GetComponent<ComponentTransform2D>();
			backgroundImage = onimaruHealthChildren[HIERARCHY_INDEX_HEALTH_BACKGROUND]->GetComponent<ComponentImage>();
			fillImage = onimaruHealthChildren[HIERARCHY_INDEX_HEALTH_FILL]->GetComponent<ComponentImage>();
			overlayImage = onimaruHealthChildren[HIERARCHY_INDEX_HEALTH_OVERLAY]->GetComponent<ComponentImage>();
			healthText = onimaruHealthChildren[HIERARCHY_INDEX_HEALTH_TEXT]->GetComponent<ComponentText>();
			switchHealthStroke = switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_STROKE_DOWN]->GetComponent<ComponentImage>();

			if (health) {
				health->SetPosition(float3::Lerp(originalOnimaruHealthPosition, originalOnimaruHealthPosition + float3(healthOffset, 0, 0), switchTimer / switchCollapseMovementTime));
			}
		}

		if (backgroundImage) {
			backgroundImage->SetColor(float4::Lerp(healthBarBackgroundColorInBackground, healthBarBackgroundColor, switchTimer / switchCollapseMovementTime));
		}

		if (fillImage) {
			fillImage->SetColor(float4::Lerp(healthFillBarColorInBackground, healthFillBarColor, switchTimer / switchCollapseMovementTime));
		}

		if (overlayImage) {
			overlayImage->SetColor(float4::Lerp(healthOverlayColorInBackground, healthOverlayColor, switchTimer / switchCollapseMovementTime));
		}

		if (healthText) {
			healthText->SetFontColor(float4::Lerp(healthTextColorInBackground, healthTextColor, switchTimer / switchCollapseMovementTime));
		}

		if (switchHealthStroke) {
			switchHealthStroke->SetColor(float4::Lerp(healthSwitchStrokeChangingColor, healthSwitchStrokeInitialColor, switchTimer / switchCollapseMovementTime));
		}

		switchHealthFillTransform2D = switchHealthChildren[HIERARCHY_INDEX_SWITCH_HEALTH_FILL]->GetComponent<ComponentTransform2D>();

		if (switchHealthFillTransform2D) {
			switchHealthFillTransform2D->SetScale(float3::Lerp(float3(0, 1, 1), float3(1, 1, 1), switchTimer / switchCollapseMovementTime));
		}

		if (switchTimer == switchDeployMovementTime) {
			switchState = SwitchState::POST_DEPLOY;
		}

		break;
	case SwitchState::POST_DEPLOY:

		if (switchTimer > switchPostDeployMovementTime) {
			switchTimer = switchPostDeployMovementTime;
		}

		if (fangSkillParent->IsActive()) {
			for (int i = 0; i < skillsFang.size(); ++i) {
				transform2D = skillsFang[i]->GetComponent<ComponentTransform2D>();
				if (transform2D) {
					transform2D->SetPosition(float3::Lerp(cooldownTransformOriginalPositions[i] + float3(switchExtraOffset + i * 10.0f, 0, 0), cooldownTransformOriginalPositions[i], switchTimer / switchPreCollapseMovementTime));
				}

			}
		} else {
			for (int i = 0; i < skillsOni.size(); ++i) {
				transform2D = skillsOni[i]->GetComponent<ComponentTransform2D>();
				if (transform2D) {
					transform2D->SetPosition(float3::Lerp(cooldownTransformOriginalPositions[i + 3] + float3(switchExtraOffset + i * 10.0f, 0, 0), cooldownTransformOriginalPositions[i + 3], switchTimer / switchPreCollapseMovementTime));
				}
			}
		}

		if (switchTimer == switchPostDeployMovementTime) {
			switchState = SwitchState::IDLE;

			std::vector<GameObject*> switchChildren = switchSkillParent->GetChildren();
			if (switchChildren.size() > HIERARCHY_INDEX_SWITCH_ABILITY_KEY_FILL - 1) {
				switchChildren[HIERARCHY_INDEX_SWITCH_ABILITY_IN_USE_WHITE]->Enable();
			}

			SetPictoState(Cooldowns::SWITCH_SKILL, PictoState::UNAVAILABLE);
			switchTimer = 0;
		}

		break;

	}
	switchTimer += Time::GetDeltaTime();
}

void HUDManager::PlayCoolDownEffect(AbilityRefeshFX* effect, Cooldowns cooldown) {
	if (effect != nullptr) {
		effect->PlayEffect();
	}
}

void HUDManager::SetPictoState(Cooldowns cooldown, PictoState newState) {
	if (pictoStates[static_cast<int>(cooldown)] == newState)return;

	float4 colorToUse = float4(0, 0, 0, 0);
	switch (newState) {
	case PictoState::AVAILABLE:
		colorToUse = cooldown == Cooldowns::SWITCH_SKILL ? switchPictoColorNotInUse : skillPictoColorAvailable;
		break;
	case PictoState::UNAVAILABLE:
		colorToUse = cooldown == Cooldowns::SWITCH_SKILL ? switchPictoColorNotInUse : skillPictoColorNotAvailable;
		break;
	case PictoState::IN_USE:
		colorToUse = cooldown == Cooldowns::SWITCH_SKILL ? switchPictoColorInUse : skillPictoColorInUse;
		break;
	}

	std::vector<GameObject*>children;

	if (static_cast<int>(cooldown) < static_cast<int>(Cooldowns::ONIMARU_SKILL_1)) {
		children = fangSkillParent->GetChildren();
	} else 	if (static_cast<int>(cooldown) < static_cast<int>(Cooldowns::SWITCH_SKILL)) {
		children = onimaruSkillParent->GetChildren();
	} else {
		children = switchSkillParent->GetChildren();
	}

	if (cooldown != Cooldowns::SWITCH_SKILL) {
		if (children[static_cast<int>(cooldown) % 3]->HasChildren()) {
			GameObject* pictoShade = children[(static_cast<int>(cooldown)) % 3]->GetChild(HIERARCHY_INDEX_ABILITY_PICTO_SHADE);

			if (pictoShade->HasChildren()) {
				GameObject* pictoFillObj = pictoShade->GetChild(static_cast<unsigned int>(0));
				if (pictoFillObj) {
					ComponentImage* pictoFill = pictoFillObj->GetComponent<ComponentImage>();
					if (pictoFill) {
						pictoFill->SetColor(colorToUse);
					}
				}
			}

		}


	} else {
		GameObject* pictoShade = children[HIERARCHY_INDEX_SWITCH_ABILITY_PICTO_SHADE];
		if (pictoShade) {
			if (pictoShade->HasChildren()) {
				GameObject* pictoFill = pictoShade->GetChild(static_cast<unsigned int>(0));
				if (pictoFill) {
					ComponentImage* pictoFillImage = pictoFill->GetComponent<ComponentImage>();
					if (pictoFillImage) {
						pictoFillImage->SetColor(colorToUse);
					}
				}
			}
		}
	}


}

void HUDManager::InitializeHealth() {
	if (!playerController) return;

	ComponentImage* health = fangHealthChildren[HIERARCHY_INDEX_HEALTH_FILL]->GetComponent<ComponentImage>();
	float healthValue = 1.f;
	if (health) {
		if (health->IsFill()) {
			healthValue = playerController->GetFangMaxHealth();
			health->SetFillValue(healthValue / healthValue);
		}
	}

	health = onimaruHealthChildren[HIERARCHY_INDEX_HEALTH_FILL]->GetComponent<ComponentImage>();
	healthValue = 1.f;
	if (health) {
		if (health->IsFill()) {
			healthValue = playerController->GetOnimaruMaxHealth();
			health->SetFillValue(healthValue / healthValue);
		}
	}

	ComponentImage* healthLost = fangHealthChildren[HIERARCHY_INDEX_HEALTH_LOST_FEEDBACK]->GetComponent<ComponentImage>();
	healthValue = 1.f;
	if (health) {
		if (health->IsFill()) {
			healthValue = playerController->GetFangMaxHealth();
			health->SetFillValue(healthValue / healthValue);
		}
	}

	healthLost = onimaruHealthChildren[HIERARCHY_INDEX_HEALTH_LOST_FEEDBACK]->GetComponent<ComponentImage>();
	healthValue = 1.f;
	if (health) {
		if (health->IsFill()) {
			healthValue = playerController->GetOnimaruMaxHealth();
			health->SetFillValue(healthValue / healthValue);
		}
	}

	ComponentText* healthText = fangHealthChildren[HIERARCHY_INDEX_HEALTH_TEXT]->GetComponent<ComponentText>();
	healthValue = playerController->GetFangMaxHealth();
	if (health) {
		healthText->SetText(std::to_string((int)healthValue));
	}

	healthText = onimaruHealthChildren[HIERARCHY_INDEX_HEALTH_TEXT]->GetComponent<ComponentText>();
	healthValue = playerController->GetOnimaruMaxHealth();
	if (health) {
		healthText->SetText(std::to_string((int)healthValue));
	}

}
