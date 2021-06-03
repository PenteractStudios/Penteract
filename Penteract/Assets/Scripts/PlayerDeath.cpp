#include "PlayerDeath.h"

#include "GameObject.h"
#include "GameplaySystems.h"

#include "PlayerController.h"

EXPOSE_MEMBERS(PlayerDeath) {
    MEMBER(MemberType::GAME_OBJECT_UID, playerUID)
};

GENERATE_BODY_IMPL(PlayerDeath);

void PlayerDeath::Start() {
	player = GameplaySystems::GetGameObject(playerUID);
}

void PlayerDeath::Update() {
	if (player) {
		PlayerController* playerController = GET_SCRIPT(player, PlayerController);
		if (playerController) {
			dead = playerController->IsDead();
		}
	}
	
}

void PlayerDeath::OnAnimationFinished() {
	if (dead) {
		SceneManager::ChangeScene("Assets/Scenes/LoseScene.scene");
	}
}