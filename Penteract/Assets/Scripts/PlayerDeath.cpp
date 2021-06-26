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
	if(player) playerController = GET_SCRIPT(player, PlayerController);
}

void PlayerDeath::Update() {
	if (player) {
		if (playerController) {
			dead = playerController->IsPlayerDead();
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
		if (playerController->playerFang.IsActive()) {
			ComponentAnimation* animation = playerController->playerFang.compAnimation;
			if (animation->GetCurrentStateSecondary()->name == "LeftShot") {
				animation->SendTriggerSecondary(playerController->playerFang.states[10] + animation->GetCurrentState()->name);
				playerController->playerFang.rightShot = true;
			}
			else if(animation->GetCurrentStateSecondary()->name == "RightShot") {
				animation->SendTriggerSecondary(playerController->playerFang.states[11] + animation->GetCurrentState()->name);
				playerController->playerFang.rightShot = false;
			}
		}
	}
}

void PlayerDeath::OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance) {
	
	if (collidedWith.name == "RangerProjectile") {
		playerController->TakeDamage(true);
	}
	else if (collidedWith.name == "MeleePunch") {
		playerController->TakeDamage(false);
	}
}
