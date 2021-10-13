#include "Enemy.h"

#include "PlayerController.h"

void Enemy::SetFallingSpeed(float fallingSpeed_) {
	fallingSpeed = fallingSpeed_;
}

void Enemy::SetSearchRadius(float searchRadius_) {
	searchRadius = searchRadius_;
}

void Enemy::SetAttackRange(float attackRange_) {
	attackRange = attackRange_;
}

void Enemy::SetTimeToDie(float timeToDie_) {
	timeToDie = timeToDie_;
}

void Enemy::IncreasePlayerUltimateCharges(PlayerController* playerController) {
	if (playerController) {
		if (playerController->playerOnimaru.characterGameObject->IsActive()) {
			playerController->playerOnimaru.IncreaseUltimateCounter();
		} else if (playerController->playerFang.characterGameObject->IsActive()) {
			playerController->playerFang.IncreaseUltimateCounter();
		}
	}
}


