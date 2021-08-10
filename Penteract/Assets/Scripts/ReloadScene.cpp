#include "ReloadScene.h"
#include "GameplaySystems.h"
#include "PlayerController.h"

EXPOSE_MEMBERS(ReloadScene) {
	MEMBER(MemberType::SCENE_RESOURCE_UID, sceneUID),
	MEMBER(MemberType::INT, levelNum)
};

GENERATE_BODY_IMPL(ReloadScene);

void ReloadScene::Start() {

}

void ReloadScene::Update() {

}

void ReloadScene::OnButtonClick() {
	if(sceneUID != 0) SceneManager::ChangeScene(sceneUID);
	PlayerController::currentLevel = levelNum;
}