#include "PlayerDeath.h"

#include "GameObject.h"
#include "GameplaySystems.h"

#include "PlayerController.h"

EXPOSE_MEMBERS(PlayerDeath) {
	MEMBER(MemberType::SCENE_RESOURCE_UID, sceneUID),
    MEMBER(MemberType::GAME_OBJECT_UID, playerUID)
};

GENERATE_BODY_IMPL(PlayerDeath);

void PlayerDeath::Start() {
	player = GameplaySystems::GetGameObject(playerUID);
	playerController = GET_SCRIPT(player, PlayerController);
}

void PlayerDeath::Update() {
	if (player) {
		if (playerController) {
			dead = playerController->IsDead();
		}
	}
	
}

void PlayerDeath::OnAnimationFinished() {
	if (dead) {
		if(sceneUID != 0) SceneManager::ChangeScene(sceneUID);
	}
		
}

void PlayerDeath::OnAnimationSecondaryFinished()
{
	if (playerController) {
		ComponentAnimation* animation = nullptr;
		State* currentState = nullptr;

		playerController->GetAnimationStatus(animation, currentState);

		if (playerController->shooting && playerController->fang->IsActive() && animation && currentState) {
			if (playerController->rightShot) {
				animation->SendTriggerSecondary(playerController->states[12] + currentState->name);
				playerController->rightShot = false;
			}
			else {
				animation->SendTriggerSecondary(playerController->states[11] + currentState->name);
				playerController->rightShot = true;
			}
		}
	}
}
