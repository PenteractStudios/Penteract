#include "GodModeController.h"

#include "GameplaySystems.h"
#include "GameObject.h"

EXPOSE_MEMBERS(GodModeController) {
	// MEMBER(MemberType::GAME_OBJECT_UID, enemySpawnPointsUID),
};

GENERATE_BODY_IMPL(GodModeController);

void GodModeController::Start() {
	gameObject = &GetOwner();
	for (GameObject* child : gameObject->GetChildren()) {
		if (child->HasComponent<ComponentScript>()) {
			children.push_back(child);
		}
	}
	Debug::Log(("Scripts: "+ std::to_string(children.size())).c_str());
}

void GodModeController::Update() {
	
}

void OnChildToggle(unsigned int index) {
	switch (index)
	{
	case 0:
		Debug::Log("0");
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