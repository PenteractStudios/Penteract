#include "HUDManager.h"
#include "PlayerController.h";

#define HIERARCHY_INDEX_ABILITY_FILL 1
#define HIERARCHY_INDEX_ABILITY_PICTO_SHADE 3
#define HIERARCHY_INDEX_ABILITY_KEY_TEXT 6
#define HIERARCHY_INDEX_ABILITY_KEY_FILL 4

#define HIERARCHY_INDEX_SWITCH_ABILITY_FILL 1
#define HIERARCHY_INDEX_SWITCH_ABILITY_PICTO_SHADE 3
#define HIERARCHY_INDEX_SWITCH_ABILITY_KEY_TEXT 6
#define HIERARCHY_INDEX_SWITCH_ABILITY_KEY_FILL 4

EXPOSE_MEMBERS(HUDManager) {
	MEMBER(MemberType::GAME_OBJECT_UID, playerObjectUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fangSkillParentUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruSkillParentUID),
	MEMBER(MemberType::GAME_OBJECT_UID, switchSkillParentUID)
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

	if (fangSkillParent && onimaruSkillParent) {
		skillsFang = fangSkillParent->GetChildren();
		skillsOni = onimaruSkillParent->GetChildren();

		for (int i = 0; i < static_cast<int>(Cooldowns::TOTAL); i++) {
			ComponentTransform2D* transform2D = nullptr;
			if (i < static_cast<int>(Cooldowns::ONIMARU_SKILL_1)) {
				//Fang skill
				transform2D = skillsFang[i]->GetComponent<ComponentTransform2D>();

			} else if (i != static_cast<int>(Cooldowns::SWITCH_SKILL)) {
				//Onimaru skill
				transform2D = skillsOni[i - 3]->GetComponent<ComponentTransform2D>();
			} else {
				transform2D = switchSkillParent->GetComponent<ComponentTransform2D>();
			}

			if (transform2D) {
				cooldownTransformOriginalPositions[i] = transform2D->GetPosition();
			}

		}
	}

}

void HUDManager::Update() {
	ManageSwitch();
}

void HUDManager::UpdateCooldowns(float onimaruCooldown1, float onimaruCooldown2, float onimaruCooldown3, float fangCooldown1, float fangCooldown2, float fangCooldown3, float switchCooldown) {
	cooldowns[static_cast<int>(Cooldowns::FANG_SKILL_1)] = fangCooldown1;
	cooldowns[static_cast<int>(Cooldowns::FANG_SKILL_2)] = fangCooldown2;
	cooldowns[static_cast<int>(Cooldowns::FANG_SKILL_3)] = fangCooldown3;
	cooldowns[static_cast<int>(Cooldowns::ONIMARU_SKILL_1)] = onimaruCooldown1;
	cooldowns[static_cast<int>(Cooldowns::ONIMARU_SKILL_2)] = onimaruCooldown2;
	cooldowns[static_cast<int>(Cooldowns::ONIMARU_SKILL_3)] = onimaruCooldown3;
	cooldowns[static_cast<int>(Cooldowns::SWITCH_SKILL)] = switchCooldown;

	if (onimaruObj && fangObj) {

		if (fangObj->IsActive()) {
			UpdateVisualCooldowns(fangSkillParent, static_cast<int>(Cooldowns::FANG_SKILL_1));

		} else {
			UpdateVisualCooldowns(onimaruSkillParent, static_cast<int>(Cooldowns::ONIMARU_SKILL_1));

		}
	}
	UpdateCommonSkillVisualCooldown();
}

void HUDManager::StartCharacterSwitch() {
	//TODO initialization
	switchTimer = 0;
	switchState = SwitchState::PRE_COLLAPSE;
}

void HUDManager::UpdateVisualCooldowns(GameObject* canvas, int startingIt) {

	std::vector<GameObject*> skills = canvas->GetChildren();

	if (skills.size() == 3) {

		int skill = startingIt;
		for (std::vector<GameObject*>::iterator it = skills.begin(); it != skills.end(); ++it) {

			ComponentImage* fillImage = nullptr;
			ComponentImage* pictogramImage = nullptr;
			ComponentText* text = nullptr;
			ComponentImage* textFill = nullptr;
			fillImage = (*it)->GetChildren()[HIERARCHY_INDEX_ABILITY_FILL]->GetComponent<ComponentImage>();
			pictogramImage = (*it)->GetChildren()[HIERARCHY_INDEX_ABILITY_PICTO_SHADE]->GetComponent<ComponentImage>();
			text = (*it)->GetChildren()[HIERARCHY_INDEX_ABILITY_KEY_TEXT]->GetComponent<ComponentText>();
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

			if (text) {
				text->SetFontColor(cooldowns[skill] < 1 ? buttonTextColorNotAvailable : buttonTextColorAvailable);
			}

			if (textFill) {
				textFill->SetColor(cooldowns[skill] < 1 ? buttonColorNotAvailable : buttonColorAvailable);
			}

			++skill;
		}

		//AbilityCoolDownEffectCheck(static_cast<Cooldowns>(skill), isMain ? canvas : nullptr);


	}
}

void HUDManager::UpdateCommonSkillVisualCooldown() {
	if (!switchSkillParent) return;


	std::vector<GameObject*> children = switchSkillParent->GetChildren();

	//TODO Check for hierarchy size

	ComponentImage* fillColor = children[HIERARCHY_INDEX_SWITCH_ABILITY_FILL]->GetComponent<ComponentImage>();
	ComponentImage* image = children[HIERARCHY_INDEX_SWITCH_ABILITY_PICTO_SHADE]->GetComponent<ComponentImage>();

	ComponentText* text = children[HIERARCHY_INDEX_ABILITY_KEY_TEXT]->GetComponent<ComponentText>();
	ComponentImage* textFill = children[HIERARCHY_INDEX_ABILITY_KEY_FILL]->GetComponent<ComponentImage>();

	if (fillColor && image) {
		fillColor->SetFillValue(cooldowns[static_cast<int>(Cooldowns::SWITCH_SKILL)]);

		fillColor->SetColor(cooldowns[static_cast<int>(Cooldowns::SWITCH_SKILL)] < 1 ? float4(switchSkillColorNotAvailable.xyz(), 0.3f + cooldowns[static_cast<int>(Cooldowns::SWITCH_SKILL)]) : switchSkillColorAvailable);

		//AbilityCoolDownEffectCheck(Cooldowns::SWITCH_SKILL, swapingSkillCanvas);
	}


	if (text) {
		text->SetFontColor(cooldowns[static_cast<int>(Cooldowns::SWITCH_SKILL)] < 1 ? buttonTextColorNotAvailable : buttonTextColorAvailable);
	}

	if (textFill) {
		textFill->SetColor(cooldowns[static_cast<int>(Cooldowns::SWITCH_SKILL)] < 1 ? buttonColorNotAvailable : buttonColorAvailable);
	}


}

void HUDManager::ManageSwitch() {
	if (!fangSkillParent || !onimaruSkillParent)return;
	if (skillsFang.size() != 3 || skillsOni.size() != 3)return;
	ComponentTransform2D* transform2D = nullptr;

	switch (switchState) {
	case SwitchState::IDLE:
		//TODO switch icon rotates over time, reset timer when rotation reaches 360º

		if (switchTimer > 2.0f) {
			switchTimer = 0;
		}

		break;
	case SwitchState::PRE_COLLAPSE:

		if (switchTimer > switchPreCollapseMovementTime) {
			switchTimer = switchPreCollapseMovementTime;
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
			switchState = SwitchState::COLLAPSE;
			switchTimer = 0;
		}

		break;
	case SwitchState::COLLAPSE:

		if (switchTimer > switchCollapseMovementTime) {
			switchTimer = switchCollapseMovementTime;
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

		if (switchTimer == switchCollapseMovementTime) {
			switchState = SwitchState::DEPLOY;

			if (fangSkillParent->IsActive()) {
				fangSkillParent->Disable();
				onimaruSkillParent->Enable();
			} else {
				fangSkillParent->Enable();
				onimaruSkillParent->Disable();
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
			switchTimer = 0;
		}

		break;

	}
	switchTimer += Time::GetDeltaTime();
}
