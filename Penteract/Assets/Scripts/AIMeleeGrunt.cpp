#include "AIMeleeGrunt.h"

#include "GameObject.h"
#include "GameplaySystems.h"

#include "PlayerController.h"
#include "HUDController.h"
#include "AIMovement.h"

EXPOSE_MEMBERS(AIMeleeGrunt) {
	MEMBER(MemberType::GAME_OBJECT_UID, playerUID),
		MEMBER(MemberType::GAME_OBJECT_UID, canvasUID),
		MEMBER(MemberType::INT, gruntCharacter.lifePoints),
		MEMBER(MemberType::FLOAT, gruntCharacter.movementSpeed),
		MEMBER(MemberType::INT, gruntCharacter.damageHit),
		MEMBER(MemberType::INT, gruntCharacter.fallingSpeed),
		MEMBER(MemberType::FLOAT, gruntCharacter.searchRadius),
		MEMBER(MemberType::FLOAT, gruntCharacter.attackRange),
		MEMBER(MemberType::FLOAT, gruntCharacter.timeToDie)
};

GENERATE_BODY_IMPL(AIMeleeGrunt);

void AIMeleeGrunt::Start() {
	player = GameplaySystems::GetGameObject(playerUID);
	if (player) {
		playerController = GET_SCRIPT(player, PlayerController);
	}
	agent = GetOwner().GetComponent<ComponentAgent>();
	if (agent) {
		agent->SetMaxSpeed(gruntCharacter.movementSpeed);
		agent->SetMaxAcceleration(static_cast<float>(AIMovement::maxAcceleration));
		agent->SetAgentObstacleAvoidance(true);
		agent->RemoveAgentFromCrowd();
	}
	animation = GetOwner().GetComponent<ComponentAnimation>();
	ownerTransform = GetOwner().GetComponent<ComponentTransform>();
	GameObject* canvas = GameplaySystems::GetGameObject(canvasUID);
	if (canvas) {
		hudControllerScript = GET_SCRIPT(canvas, HUDController);
	}
	movementScript = GET_SCRIPT(&GetOwner(), AIMovement);
}

void AIMeleeGrunt::Update() {
	if (!GetOwner().IsActive()) return;
	if (!player) return;
	if (!agent) return;
	if (!movementScript) return;
	if (!hudControllerScript) return;
	if (!playerController) return;
	if (!ownerTransform) return;
	if (!animation) return;

	if (agent) {
		agent->SetMaxSpeed(gruntCharacter.movementSpeed);
	}

	if (hitTaken && gruntCharacter.isAlive) {
		gruntCharacter.Hit(damageRecieved);
		hitTaken = false;
	}

	if (!gruntCharacter.isAlive) {
		if (state == AIState::ATTACK) {
			animation->SendTrigger("AttackDeath");
		} else if (state == AIState::IDLE) {
			animation->SendTrigger("IdleDeath");
		} else if (state == AIState::RUN) {
			animation->SendTrigger("RunDeath");
		}
		agent->RemoveAgentFromCrowd();
		state = AIState::DEATH;
	}

	switch (state) {
	case AIState::START:
		if (Camera::CheckObjectInsideFrustum(GetOwner().GetChildren()[0])) {
			movementScript->Seek(state, float3(ownerTransform->GetGlobalPosition().x, 0, ownerTransform->GetGlobalPosition().z), static_cast<int>(gruntCharacter.fallingSpeed), true);
			if (ownerTransform->GetGlobalPosition().y < 2.7 + 0e-5f) {
				animation->SendTrigger("StartSpawn");
				state = AIState::SPAWN;
			}
		}
		break;
	case AIState::SPAWN:
		break;
	case AIState::IDLE:
		if (!playerController->IsDead()) {
			if (movementScript->CharacterInSight(player, gruntCharacter.searchRadius)) {
				animation->SendTrigger("IdleRun");
				state = AIState::RUN;
			}
		}
		break;
	case AIState::RUN:
		movementScript->Seek(state, player->GetComponent<ComponentTransform>()->GetGlobalPosition(), static_cast<int>(gruntCharacter.movementSpeed), true);
		if (movementScript->CharacterInMeleeRange(player, gruntCharacter.attackRange)) {
			agent->RemoveAgentFromCrowd();
			animation->SendTrigger("RunAttack");
			state = AIState::ATTACK;
		}
		break;
	case AIState::ATTACK:
		break;
	case AIState::DEATH:
		break;
	}

	if (!gruntCharacter.isAlive) {
		if (gruntCharacter.timeToDie > 0) {
			gruntCharacter.timeToDie -= Time::GetDeltaTime();
		} else {
			if (hudControllerScript) {
				hudControllerScript->UpdateScore(10);
			}
			GameplaySystems::DestroyGameObject(&GetOwner());
		}
	}

}

void AIMeleeGrunt::OnAnimationFinished() {
	if (state == AIState::SPAWN) {
		animation->SendTrigger("SpawnIdle");
		state = AIState::IDLE;
		agent->AddAgentToCrowd();
	}

	else if (state == AIState::ATTACK) {
		playerController->HitDetected(gruntCharacter.damageHit);
		animation->SendTrigger("AttackIdle");
		agent->AddAgentToCrowd();
		state = AIState::IDLE;
	}

	else if (state == AIState::DEATH) {
		gruntCharacter.isAlive = false;
	}

}

void AIMeleeGrunt::HitDetected(int damage_) {
	damageRecieved = damage_;
	hitTaken = true;
}
