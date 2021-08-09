#include "PlayerDeath.h"

#include "GameObject.h"
#include "GameplaySystems.h"

#include "PlayerController.h"
#include "SceneTransition.h"

#define LEFT_SHOT "LeftShot"
#define RIGHT_SHOT "RightShot"

EXPOSE_MEMBERS(PlayerDeath) {
	MEMBER(MemberType::SCENE_RESOURCE_UID, sceneUID),
	MEMBER(MemberType::GAME_OBJECT_UID, playerUID),
	MEMBER(MemberType::FLOAT, rangedDamageTaken),
	MEMBER(MemberType::FLOAT, meleeDamageTaken),
	MEMBER(MemberType::FLOAT, barrelDamageTaken),
	MEMBER(MemberType::FLOAT, fireDamageTaken),
	MEMBER(MemberType::FLOAT, cooldownFireDamage),
	MEMBER(MemberType::GAME_OBJECT_UID, transitionUID)
};

GENERATE_BODY_IMPL(PlayerDeath);

void PlayerDeath::Start() {
	player = GameplaySystems::GetGameObject(playerUID);
	if (player) playerController = GET_SCRIPT(player, PlayerController);
	if (transitionUID != 0) {
		transitionGO = GameplaySystems::GetGameObject(transitionUID);
		if (transitionGO) sceneTransition = GET_SCRIPT(transitionGO, SceneTransition);
	}
}

void PlayerDeath::Update() {
	if (player) {
		if (playerController) {
			dead = playerController->IsPlayerDead();
		}
		if (timerFireDamage <= cooldownFireDamage) {
			timerFireDamage += Time::GetDeltaTime();
			if (timerFireDamage > cooldownFireDamage) {
				fireDamageActive = true;
				timerFireDamage = 0.f;
			}
		}
	}
}

void PlayerDeath::OnAnimationFinished() {
	if (dead) {
		if (sceneTransition) {
			sceneTransition->StartTransition();
		}
		else {
			if (sceneUID != 0) SceneManager::ChangeScene(sceneUID);
		}
	}
	else {
		if (!playerController)return;
		if (playerController->playerFang.characterGameObject->IsActive()) {
			playerController->playerFang.OnAnimationFinished();
		}
		else {
			playerController->playerOnimaru.OnAnimationFinished();
		}
	}
}

void PlayerDeath::OnAnimationSecondaryFinished() {
	if (playerController) {
		if (playerController->playerFang.IsActive()) {
			ComponentAnimation* animation = playerController->playerFang.compAnimation;
			if (animation->GetCurrentState() && animation->GetCurrentStateSecondary()) {
				if (animation->GetCurrentStateSecondary()->name == LEFT_SHOT) {
					animation->SendTriggerSecondary(playerController->playerFang.states[10] + animation->GetCurrentState()->name);
					playerController->playerFang.rightShot = true;
				}
				else if (animation->GetCurrentStateSecondary()->name == RIGHT_SHOT) {
					animation->SendTriggerSecondary(playerController->playerFang.states[11] + animation->GetCurrentState()->name);
					playerController->playerFang.rightShot = false;
				}
			}
		}
		else {
			playerController->playerOnimaru.OnAnimationSecondaryFinished();
		}
	}
}

void PlayerDeath::OnAnimationEvent(StateMachineEnum stateMachineEnum, const char* eventName) {
	if (playerController) {
		if (playerController->playerFang.IsActive()) {
			playerController->playerFang.OnAnimationEvent(stateMachineEnum, eventName);
		} else {
			playerController->playerOnimaru.OnAnimationEvent(stateMachineEnum, eventName);
		}
	}
}

void PlayerDeath::OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle) {
	if (collidedWith.name == "RangerProjectile") {
		if(playerController) playerController->TakeDamage(rangedDamageTaken);
	}
	else if (collidedWith.name == "MeleePunch") {
		if(playerController) playerController->TakeDamage(meleeDamageTaken);
	}
	else if (collidedWith.name == "Barrel") {
		if(playerController) playerController->TakeDamage(barrelDamageTaken);
	}
	else if (collidedWith.name == "FireTile") {
		if (fireDamageActive) {
			if (playerController) playerController->TakeDamage(fireDamageTaken);
			fireDamageActive = false;
		}
	}
}