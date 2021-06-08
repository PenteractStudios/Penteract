#include "GameManager.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(GameManager) {
    // Add members here to expose them to the engine. Example:
    MEMBER(MemberType::GAME_OBJECT_UID, gameManagerUID),
    MEMBER(MemberType::FLOAT, volumeScene)
};

GENERATE_BODY_IMPL(GameManager);

void GameManager::Start() {
    volumeScene = GameManager::GetInstance()->GetVolumeScene();
}

void GameManager::Update() {
    volumeScene = GameManager::GetInstance()->GetVolumeScene();
}