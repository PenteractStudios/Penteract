#include "RangedAI.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "PlayerController.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAgent.h"

EXPOSE_MEMBERS(RangedAI) {
	MEMBER(MemberType::GAME_OBJECT_UID, playerUID),
		MEMBER(MemberType::GAME_OBJECT_UID, playerMeshUIDFang),
		MEMBER(MemberType::GAME_OBJECT_UID, playerMeshUIDOnimaru),
		MEMBER(MemberType::GAME_OBJECT_UID, meshUID),
		MEMBER(MemberType::GAME_OBJECT_UID, meshUID1),
		MEMBER(MemberType::GAME_OBJECT_UID, meshUID2),
		MEMBER(MemberType::INT, maxMovementSpeed),
		MEMBER(MemberType::INT, lifePoints),
		MEMBER(MemberType::FLOAT, searchRadius),
		MEMBER(MemberType::FLOAT, attackRange),
		MEMBER(MemberType::FLOAT, timeToDie),
		MEMBER(MemberType::FLOAT, attackSpeed),
		MEMBER(MemberType::FLOAT, fleeingRange),
		MEMBER(MemberType::GAME_OBJECT_UID, agentObjectUID),
		MEMBER(MemberType::BOOL, foundRayToPlayer)
};

GENERATE_BODY_IMPL(RangedAI);

void RangedAI::Start() {
	player = GameplaySystems::GetGameObject(playerUID);
	meshObj = GameplaySystems::GetGameObject(meshUID);
	meshObj1 = GameplaySystems::GetGameObject(meshUID1);
	meshObj2 = GameplaySystems::GetGameObject(meshUID2);
	animation = GetOwner().GetParent()->GetComponent<ComponentAnimation>();
	parentTransform = GetOwner().GetParent()->GetComponent<ComponentTransform>();

	ComponentBoundingBox* bb = meshObj->GetComponent<ComponentBoundingBox>();
	bbCenter = (bb->GetLocalMinPointAABB() + bb->GetLocalMaxPointAABB()) / 2;

	agent = GameplaySystems::GetGameObject(agentObjectUID)->GetComponent<ComponentAgent>();

}

void RangedAI::OnAnimationFinished() {
	if (state == RangeAIState::SPAWN) {
		animation->SendTrigger("SpawnIdle");
		state = RangeAIState::IDLE;
	} else if (state == RangeAIState::HURT && lifePoints > 0) {
		animation->SendTrigger("HurtIdle");
		state = RangeAIState::IDLE;
	}

	else if (state == RangeAIState::HURT && lifePoints <= 0) {
		//animation->SendTrigger("HurtDeath");
		Debug::Log("Death");
		state = RangeAIState::DEATH;
	} else if (state == RangeAIState::DEATH) {
		dead = true;
	}
}

void RangedAI::Update() {
	if (!GetOwner().IsActive()) return;

	if (hitTaken && lifePoints > 0) {
		if (state == RangeAIState::IDLE || state == RangeAIState::HURT) {
			animation->SendTrigger("IdleHurt");
		} else if (state == RangeAIState::FLEE) {
			animation->SendTrigger("RunHurt");
		}

		lifePoints -= damageRecieved;
		state = RangeAIState::HURT;
		hitTaken = false;
	}

	if (state == RangeAIState::IDLE || state == RangeAIState::APPROACH) {
		attackTimePool = Max(attackTimePool - Time::GetDeltaTime(), 0.0f);
		if (attackTimePool == 0) {
			ShootPlayerInRange();
		}
	}

	UpdateState();
}

void RangedAI::EnterState(RangeAIState newState) {
	switch (newState) {
	case RangeAIState::START:
		ChangeState(RangeAIState::IDLE);
		break;
	case RangeAIState::SPAWN:
		ChangeState(RangeAIState::IDLE);
		break;
	case RangeAIState::IDLE:
		StopMovement();
		break;
	case RangeAIState::FLEE:
		animation->SendTrigger("IdleRun");
		break;
	case RangeAIState::HURT:

		break;
	case RangeAIState::DEATH:

		break;
	}
}

void RangedAI::UpdateState() {
	switch (state) {
	case RangeAIState::START:
		ChangeState(RangeAIState::IDLE);
		break;
	case RangeAIState::SPAWN:
		ChangeState(RangeAIState::IDLE);
		break;
	case RangeAIState::SHOOT:

		break;
	case RangeAIState::IDLE:
		//Shooting will probably happen here
		if (player) {
			if (CharacterInSight(player)) {
				if (CharacterTooClose(player)) {
					ChangeState(RangeAIState::FLEE);
					break;
				}
				if (CharacterShootable(player, false)) {
					OrientateTo(player->GetComponent<ComponentTransform>()->GetGlobalPosition() - parentTransform->GetGlobalPosition());
				} else {
					ChangeState(RangeAIState::APPROACH);
				}
			}
		}
		break;
	case RangeAIState::APPROACH:
		if (CharacterInSight(player)) {
			if (!CharacterInRange(player) && !CharacterShootable(player, false)) {
				Seek(player->GetComponent<ComponentTransform>()->GetGlobalPosition(), maxMovementSpeed);
			} else {
				ChangeState(RangeAIState::IDLE);
			}
		}
		break;
	case RangeAIState::FLEE:

		if (CharacterTooClose(player)) {
			Flee(player->GetComponent<ComponentTransform>()->GetGlobalPosition(), maxMovementSpeed);
		} else {
			ChangeState(RangeAIState::IDLE);
		}
		break;
	case RangeAIState::HURT:
		if (timeStunned > maxStunnedTime) {
			ChangeState(RangeAIState::IDLE);
			timeStunned = 0;
		} else {
			timeStunned += Time::GetDeltaTime();
		}
		break;
	case RangeAIState::DEATH:
		break;
	default:
		break;
	}
}

void RangedAI::ExitState() {

}


void RangedAI::HitDetected(int damage_) {
	damageRecieved = damage_;
	hitTaken = true;
}

void RangedAI::ChangeState(RangeAIState newState) {
	ExitState();
	EnterState(newState);
	state = newState;
	std::string message = "Entering state " + StateToString(newState);
	Debug::Log(message.c_str());
}

bool RangedAI::CharacterInSight(const GameObject* character) {
	ComponentTransform* target = character->GetComponent<ComponentTransform>();
	if (target) {
		float3 posTarget = target->GetGlobalPosition();
		return posTarget.Distance(parentTransform->GetGlobalPosition()) < searchRadius;
	}

	return false;
}

bool RangedAI::CharacterInRange(const GameObject* character) {

	if (meshObj) return Camera::CheckObjectInsideFrustum(meshObj) && Camera::CheckObjectInsideFrustum(meshObj1) && Camera::CheckObjectInsideFrustum(meshObj2);

	ComponentTransform* target = character->GetComponent<ComponentTransform>();
	if (target) {
		float3 posTarget = target->GetGlobalPosition();
		return posTarget.Distance(parentTransform->GetGlobalPosition()) < attackRange;
	}

}

bool RangedAI::CharacterShootable(const GameObject* character, bool useForward) {
	float3 start = parentTransform->GetGlobalPosition();// +parentTransform->GetLocalMatrix().Mul(float4(bbCenter, 1)).xyz();

	float3 dir = float3(0, 0, 0);

	float3 playerMeshBoundingBoxCenter = float3(0, 0, 0);
	//ComponentBoundingBox* bb = nullptr;
	ComponentTransform* activePlayerMeshTransform = nullptr;

	GameObject* activePlayerMeshObj = nullptr;

	activePlayerMeshObj = GameplaySystems::GetGameObject(playerMeshUIDFang);

	if (!activePlayerMeshObj->IsActive()) {
		activePlayerMeshObj = GameplaySystems::GetGameObject(playerMeshUIDOnimaru);
	}

	//bb = activePlayerMeshObj->GetComponent<ComponentBoundingBox>();
	activePlayerMeshTransform = activePlayerMeshObj->GetComponent<ComponentTransform>();

	//if (bb) {
	//	playerMeshBoundingBoxCenter = (bb->GetLocalMinPointAABB() + bb->GetLocalMaxPointAABB()) / 2;
	//	playerMeshBoundingBoxCenter = activePlayerMeshTransform->GetGlobalMatrix().Mul(float4(playerMeshBoundingBoxCenter, 1)).xyz();
	//}

	if (useForward) {
		dir = parentTransform->GetGlobalRotation() * float3(0, 0, 1);
	} else {
		dir = activePlayerMeshTransform->GetGlobalPosition() + playerMeshBoundingBoxCenter - start;
	}

	dir.Normalize();
	int mask = static_cast<int>(MaskType::PLAYER);
	GameObject* hitGo = Physics::Raycast(start, start + dir * attackRange, mask);

	//std::string message = "Ray from: ";
	//message += std::to_string(start.x);
	//message += ",";
	//message += std::to_string(start.y);
	//message += ",";
	//message += std::to_string(start.z);
	//message += " with direction: ";
	//message += std::to_string(dir.x);
	//message += ",";
	//message += std::to_string(dir.y);
	//message += ",";
	//message += std::to_string(dir.z);

	//if (hitGo) {
	//	message += " COLLISION FOUND";

	//}

	//Debug::Log(message.c_str());

	return foundRayToPlayer;

	if (hitGo == activePlayerMeshObj) {

		return true;
	}
	return false;
}

bool RangedAI::CharacterTooClose(const GameObject* character) {
	ComponentTransform* target = character->GetComponent<ComponentTransform>();
	if (target) {
		float3 posTarget = target->GetGlobalPosition();
		return posTarget.Distance(parentTransform->GetGlobalPosition()) < fleeingRange;
	}

	return false;
}

void RangedAI::Seek(const float3& newPosition, int speed) {

	if (!agent) {
		float3 position = parentTransform->GetGlobalPosition();
		float3 direction = newPosition - position;

		velocity = direction.Normalized() * speed;

		position += velocity * Time::GetDeltaTime();

		parentTransform->SetGlobalPosition(position);

		OrientateTo(direction);

	} else {
		agent->SetMoveTarget(newPosition, true);
		OrientateTo(agent->GetVelocity());
	}
}

void RangedAI::Flee(const float3& fromPosition, int speed) {
	if (!agent) {

		float3 position = parentTransform->GetGlobalPosition();
		float3 direction = position - fromPosition;

		velocity = direction.Normalized() * speed;

		position += velocity * Time::GetDeltaTime();

		parentTransform->SetGlobalPosition(position);

		Quat newRotation = Quat::LookAt(float3(0, 0, 1), direction.Normalized(), float3(0, 1, 0), float3(0, 1, 0));
		parentTransform->SetGlobalRotation(newRotation);

	} else {
		float3 position = parentTransform->GetGlobalPosition();
		float3 direction = (position - fromPosition).Normalized() * fleeingEvaluateDistance;

		agent->SetMoveTarget(direction, true);

		OrientateTo(agent->GetVelocity());
		//agent->SetMoveTarget(newPosition, true);
		//Quat newRotation = Quat::LookAt(float3(0, 0, 1), agent->GetVelocity().Normalized(), float3(0, 1, 0), float3(0, 1, 0));
		//parentTransform->SetGlobalRotation(newRotation);
	}
}

void RangedAI::StopMovement() {
	if (agent) {
		agent->SetMoveTarget(GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition(), true);
	}
}

void RangedAI::OrientateTo(const float3& direction) {
	Quat newRotation = Quat::LookAt(float3(0, 0, 1), direction.Normalized(), float3(0, 1, 0), float3(0, 1, 0));
	parentTransform->SetGlobalRotation(newRotation);
}

std::string RangedAI::StateToString(RangeAIState state) {
	switch (state) {
	case RangeAIState::START:
		return "Start";
	case RangeAIState::SPAWN:
		return "Spawn";
	case RangeAIState::IDLE:
		return "Idle";
	case RangeAIState::APPROACH:
		return "Approach";
	case RangeAIState::SHOOT:
		return "Shoot";
	case RangeAIState::FLEE:
		return "Flee";
	case RangeAIState::HURT:
		return "Hurt";
	case RangeAIState::DEATH:
		return "Death";
	}
}

void RangedAI::ShootPlayerInRange() {
	if (!player) return;
	if (CharacterInRange(player)) {
		Debug::Log("Shoot");
		attackTimePool = 1.0f / attackSpeed;

		if (shootAudioSource)
			shootAudioSource->Play();
		//if (fang->IsActive()) {
		//	fangCompParticle->Play();
		//} else {
		//	onimaruCompParticle->Play();
		//}

		float3 start = parentTransform->GetGlobalPosition() + bbCenter;
		float3 end = parentTransform->GetGlobalRotation() * float3(0, 0, 1);
		end.Normalize();
		end *= attackRange;
		int mask = static_cast<int>(MaskType::PLAYER);
		GameObject* hitGo = Physics::Raycast(start, start + end, mask);
		if (hitGo) {

			//AIMovement* enemyScript = GET_SCRIPT(hitGo, AIMovement);
			//if (fang->IsActive()) enemyScript->HitDetected(3);
			//else enemyScript->HitDetected();
			std::string message = "HitGo " + hitGo->name;
			Debug::Log(message.c_str());

		}


	}
}
