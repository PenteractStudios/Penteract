#include "GodModeController.h"
#include "SpawnOnClick.h"

#include "GameplaySystems.h"
#include "GameObject.h"

#include "Components/UI/ComponentToggle.h"

EXPOSE_MEMBERS(GodModeController) {
	MEMBER(MemberType::GAME_OBJECT_UID, uiCanvasUID),
		MEMBER(MemberType::GAME_OBJECT_UID, spawnMeleeUID),
		MEMBER(MemberType::GAME_OBJECT_UID, spawnRangedUID),
		MEMBER(MemberType::GAME_OBJECT_UID, toggleEnemiesUID),
		MEMBER(MemberType::GAME_OBJECT_UID, invincibleUID),
};

GENERATE_BODY_IMPL(GodModeController);

void GodModeController::Start() {
	gameObject = &GetOwner();
	uiCanvas = GameplaySystems::GetGameObject(uiCanvasUID);
	spawnMelee = GameplaySystems::GetGameObject(spawnMeleeUID);
	spawnRanged = GameplaySystems::GetGameObject(spawnRangedUID);
	toggleEnemies = GameplaySystems::GetGameObject(toggleEnemiesUID);
	invincible = GameplaySystems::GetGameObject(invincibleUID);

	for (GameObject* child : uiCanvas->GetChildren()) {
		if (child->HasComponent<ComponentToggle>()) {
			ComponentToggle* toggle = child->GetComponent<ComponentToggle>();
			if (toggle) {
				Debug::Log(toggle->IsChecked() ? "is checked" : "isn't checked");
				toggles.push_back(toggle);
			}
		}
	}
}

void GodModeController::Update() {}

void GodModeController::OnChildToggle(unsigned int index, bool isChecked) {
	switch (index) {
	case 0:
		if (isChecked) {
			if (toggles[index + 1]->IsChecked()) {
				toggles[index + 1]->SetChecked(false);
			}
			spawnMelee->Enable();
		}
		else {
			spawnMelee->Disable();
		}
		break;
	case 1:
		if (isChecked) {
			if (toggles[index - 1]->IsChecked()) {
				toggles[index - 1]->SetChecked(false);
			}
			spawnRanged->Enable();
		}
		else {
			spawnRanged->Disable();
		}
		break;
	case 2:
		if (isChecked) {
			toggleEnemies->Enable();
		}
		else {
			toggleEnemies->Disable();
		}
		break;
	case 3:
		if (isChecked) {
			invincible->Enable();
		}
		else {
			invincible->Disable();
		}
		break;

	default:
		break;
	}
}