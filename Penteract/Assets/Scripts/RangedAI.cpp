#include "RangedAI.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "PlayerController.h"
#include "HUDController.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAgent.h"
#include "Components/ComponentAnimation.h"
#include "Resources/ResourcePrefab.h"

//clang-format off
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
		MEMBER(MemberType::BOOL, foundRayToPlayer),
		MEMBER(MemberType::FLOAT, fleeingEvaluateDistance),
		MEMBER(MemberType::PREFAB_RESOURCE_UID, trailPrefabUID),
		MEMBER(MemberType::GAME_OBJECT_UID, hudControllerObjUID),
		MEMBER(MemberType::FLOAT, approachOffset) //This variable should be a positive float, it will be used to make AIs get a bit closer before stopping their approach

};//clang-format on


GENERATE_BODY_IMPL(RangedAI);

void RangedAI::Start() {
	player = GameplaySystems::GetGameObject(playerUID);
	meshObj = GameplaySystems::GetGameObject(meshUID);
	meshObj1 = GameplaySystems::GetGameObject(meshUID1);
	meshObj2 = GameplaySystems::GetGameObject(meshUID2);
	animation = GetOwner().GetComponent<ComponentAnimation>();
	parentTransform = GetOwner().GetComponent<ComponentTransform>();
	fangMeshObj = GameplaySystems::GetGameObject(playerMeshUIDFang);
	onimaruMeshObj = GameplaySystems::GetGameObject(playerMeshUIDOnimaru);
	ComponentBoundingBox* bb = meshObj->GetComponent<ComponentBoundingBox>();
	bbCenter = (bb->GetLocalMinPointAABB() + bb->GetLocalMaxPointAABB()) / 2;
	shootTrailPrefab = GameplaySystems::GetResource<ResourcePrefab>(trailPrefabUID);

	GameObject* hudControllerObj = GameplaySystems::GetGameObject(hudControllerObjUID);
	if (hudControllerObj) {
		hudControllerScript = GET_SCRIPT(hudControllerObj, HUDController);
	}

	if (player) {
		playerController = GET_SCRIPT(player, PlayerController);
	}

	// TODO: ADD CHECK PLS
	agent = GameplaySystems::GetGameObject(agentObjectUID)->GetComponent<ComponentAgent>();

}

void RangedAI::OnAnimationFinished() {
	if (animation == nullptr) return;
	if (state == RangeAIState::SPAWN) {
		animation->SendTrigger("SpawnIdle");
		state = RangeAIState::IDLE;
	} else if (state == RangeAIState::DEATH) {
		dead = true;
	}
}

void RangedAI::Update() {
	if (!GetOwner().IsActive()) return;

	if (hitTaken && lifePoints > 0) {
		lifePoints -= damageRecieved;
		hitTaken = false;
		if (lifePoints <= 0) {
			ChangeState(RangeAIState::DEATH);
		}
	}

	if (state == RangeAIState::IDLE || state == RangeAIState::APPROACH || state == RangeAIState::FLEE) {
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
	case RangeAIState::DEATH:
		if (state == RangeAIState::IDLE) {
			animation->SendTrigger("IdleDeath");
		} else if (state == RangeAIState::APPROACH) {
			animation->SendTrigger("RunDeath");
		}
		Debug::Log("Death");
		agent->RemoveAgentFromCrowd();
		state = RangeAIState::DEATH;
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

				if (!CharacterInRange(player, attackRange, true)) {
					ChangeState(RangeAIState::APPROACH);
					break;
				}

				if (FindsRayToCharacter(player, false)) {
					OrientateTo(player->GetComponent<ComponentTransform>()->GetGlobalPosition() - parentTransform->GetGlobalPosition());
				} else {
					ChangeState(RangeAIState::APPROACH);
				}
			}
		}
		break;
	case RangeAIState::APPROACH:
		if (CharacterInSight(player)) {
			if (!CharacterInRange(player, attackRange - approachOffset, true) || !FindsRayToCharacter(player, false)) {
				if (!CharacterTooClose(player)) {
					Seek(player->GetComponent<ComponentTransform>()->GetGlobalPosition(), maxMovementSpeed);
				}
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
	case RangeAIState::DEATH:
		if (dead) {
			if (dead) {
				if (timeToDie > 0) {
					timeToDie -= Time::GetDeltaTime();
				} else {
					if (hudControllerScript) {
						hudControllerScript->UpdateScore(10);
					}
					GameplaySystems::DestroyGameObject(&GetOwner());
				}
			}
		}

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

bool RangedAI::CharacterInRange(const GameObject* character, float range, bool useRange) {

	if (meshObj) {
		if (!useRange) {
			return Camera::CheckObjectInsideFrustum(meshObj) && Camera::CheckObjectInsideFrustum(meshObj1) && Camera::CheckObjectInsideFrustum(meshObj2);
		}
	}

	ComponentTransform* target = character->GetComponent<ComponentTransform>();
	if (target) {
		float3 posTarget = target->GetGlobalPosition();
		return posTarget.Distance(parentTransform->GetGlobalPosition()) < range;
	}
	return false;
}

bool RangedAI::FindsRayToCharacter(const GameObject* character, bool useForward) {
	ComponentBoundingBox* box = meshObj->GetComponent<ComponentBoundingBox>();
	float3 offset(0, 0, 0);

	if (box) {
		float y = (box->GetWorldAABB().maxPoint + box->GetWorldAABB().minPoint).y / 4;
		offset.y = y;
	}


	float3 start = GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition() + offset;

	GameObject* activePlayerMeshObj = fangMeshObj;

	if (!activePlayerMeshObj->IsActive()) {
		activePlayerMeshObj = onimaruMeshObj;
	}

	float3 dir = GetOwner().GetComponent<ComponentTransform>()->GetGlobalRotation() * float3(0, 0, 1);

	if (!useForward) {
		dir = activePlayerMeshObj->GetComponent<ComponentTransform>()->GetGlobalPosition() - start;
		dir.y = 0;
		dir.Normalize();
	}

	int mask = static_cast<int>(MaskType::PLAYER);
	GameObject* hitGo = Physics::Raycast(start, start + dir * attackRange, mask);

	//if (hitGo) {
	//	Debug::Log("Hit!");
	//}

	return hitGo != nullptr;

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
}

bool RangedAI::CharacterTooClose(const GameObject* character) {

	return CharacterInRange(character, fleeingRange, true);
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
		float3 direction = (position - fromPosition).Normalized();
		float3 newPosition = position + direction * fleeingEvaluateDistance;
		agent->SetMoveTarget(newPosition, true);
		OrientateTo(agent->GetVelocity());
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
	case RangeAIState::DEATH:
		return "Death";
	}
}

void RangedAI::ShootPlayerInRange() {
	if (!player) return;
	if (CharacterInRange(player, attackRange, true)) {
		Debug::Log("Shoot");

		if (shootTrailPrefab) {
			//TODO WAIT STRETCH FROM LOWY AND IMPLEMENT SOME SHOOT EFFECT
			ComponentBoundingBox* box = meshObj->GetComponent<ComponentBoundingBox>();
			float offsetY = (box->GetWorldAABB().minPoint.y + box->GetWorldAABB().maxPoint.y) / 4;

			GameplaySystems::Instantiate(shootTrailPrefab, GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition() + float3(0, offsetY, 0), GetOwner().GetComponent<ComponentTransform>()->GetGlobalRotation());
			float3 frontTrail = GetOwner().GetComponent<ComponentTransform>()->GetGlobalRotation() * float3(0.0f, 0.0f, 1.0f);
			GameplaySystems::Instantiate(shootTrailPrefab, GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition() + float3(0, offsetY, 0), Quat::RotateAxisAngle(frontTrail, (pi / 2)).Mul(GetOwner().GetComponent<ComponentTransform>()->GetGlobalRotation()));
		}

		attackTimePool = 1.0f / attackSpeed;

		if (shootAudioSource)
			shootAudioSource->Play();



		if (FindsRayToCharacter(player, true)) {
			Debug::Log("Player was shot");
		}

		//if (fang->IsActive()) {
		//	fangCompParticle->Play();
		//} else {
		//	onimaruCompParticle->Play();
		//}

		//float3 start = parentTransform->GetGlobalPosition() + bbCenter;
		//float3 end = parentTransform->GetGlobalRotation() * float3(0, 0, 1);
		//end.Normalize();
		//end *= attackRange;
		//int mask = static_cast<int>(MaskType::PLAYER);
		//GameObject* hitGo = Physics::Raycast(start, start + end, mask);
		//if (hitGo) {

		//	//AIMovement* enemyScript = GET_SCRIPT(hitGo, AIMovement);
		//	//if (fang->IsActive()) enemyScript->HitDetected(3);
		//	//else enemyScript->HitDetected();
		//	std::string message = "HitGo " + hitGo->name;
		//	Debug::Log(message.c_str());

		//}


	}
}
