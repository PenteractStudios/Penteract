#include "GodModeController.h"

#include "GameplaySystems.h"
#include "GameObject.h"

#include "Components/UI/ComponentToggle.h"

EXPOSE_MEMBERS(GodModeController) {
	MEMBER(MemberType::GAME_OBJECT_UID, uiCanvasUID)
};

GENERATE_BODY_IMPL(GodModeController);

void GodModeController::Start() {
	gameObject = &GetOwner();
	uiCanvas = GameplaySystems::GetGameObject(uiCanvasUID);

	for (GameObject* child : gameObject->GetChildren()) {
		if (child->HasComponent<ComponentScript>()) {
			children.push_back(child);
		}
	}

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
		Debug::Log("0");
		if (isChecked) {
			if (toggles[index + 1]->IsChecked()) {
				toggles[index + 1]->SetChecked(false);
			}
		}

		break;
	case 1:
		Debug::Log("1");
		if (isChecked) {
			if (toggles[index - 1]->IsChecked()) {
				toggles[index - 1]->SetChecked(false);
			}
		}
		break;
	case 2:
		Debug::Log("2");
		break;
	case 3:
		Debug::Log("3");
		break;

	default:
		break;
	}
}

// void GodModeController::SpawnEnemies() {
// 	GameObject* spawnGO = GameplaySystems::GetGameObject(enemySpawnPointsUID);
// 	for (GameObject* child : spawnGO->GetChildren()) {
// 		if (child->IsActive()) {
// 			child->Enable();
// 		}
// 		else {
// 			child->Disable();
// 		}
// 	}
// }