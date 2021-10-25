#include "ReloadScene.h"
#include "GameplaySystems.h"
#include "GlobalVariables.h"
#include "PlayerController.h"
#include "PauseController.h"
#include "GameController.h"

EXPOSE_MEMBERS(ReloadScene) {
	MEMBER(MemberType::SCENE_RESOURCE_UID, sceneUID),
	MEMBER(MemberType::GAME_OBJECT_UID, gameControllerUID)
};

GENERATE_BODY_IMPL(ReloadScene);

void ReloadScene::Start() {
	lastLoadCheckpoint = GameplaySystems::GetGlobalVariable(globalCheckpoint, 0);
}

void ReloadScene::Update() {}

void ReloadScene::OnButtonClick() {
	if (sceneUID != 0) {
		SceneManager::ChangeScene(sceneUID);
		int actualCP = GameplaySystems::GetGlobalVariable(globalCheckpoint, 0);
		if (actualCP < lastLoadCheckpoint) {
			GameplaySystems::SetGlobalVariable(globalCheckpoint, lastLoadCheckpoint);
		}
	}
	PlayerController::currentLevel = GameplaySystems::GetGlobalVariable(globalLevel,1);

	// If this is clicked from a Menu Button in which everything is frozen, this should resume it
	PauseController::SetIsPause(false);
	GameObject* gameController = GameplaySystems::GetGameObject(gameControllerUID);
	if (gameController) {
		GameController* gameControllerScript = GET_SCRIPT(gameController, GameController);
		if (gameControllerScript) {
			gameControllerScript->ResumeGame();
		}
	}
}