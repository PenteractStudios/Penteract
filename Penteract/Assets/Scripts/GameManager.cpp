#include "GameManager.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(GameManager) {
    // Add members here to expose them to the engine. Example:
    // MEMBER(MemberType::BOOL, exampleMember1),
    // MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
    //MEMBER(MemberType::GAME_OBJECT_UID, sceneUID),
    MEMBER(MemberType::GAME_OBJECT_UID, gameManagerUID),
    MEMBER(MemberType::FLOAT, volumeScene)
};

GENERATE_BODY_IMPL(GameManager);

void GameManager::Start() {
    Awake();
}

void GameManager::Update() {
	
}

void GameManager::Awake()
{
    // Seek if we have in the scene already an a GameManager
    gameManager = GameplaySystems::GetGameObject(gameManagerUID);
    if (gameManager) {
        // Destroy the gameObject in the scene
        gameManager = nullptr;
        //App->scene->DestroyGameObjectDeferred(gameManager);
    } else {

    }
}
