#include "RangedAI.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "PlayerController.h"
#include "HUDController.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAgent.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentMeshRenderer.h"
#include "AIMovement.h"
#include "RangerProjectileScript.h"
#include "Resources/ResourcePrefab.h"

//clang-format off
EXPOSE_MEMBERS(RangedAI) {
	MEMBER(MemberType::GAME_OBJECT_UID, playerUID),
		MEMBER(MemberType::GAME_OBJECT_UID, playerMeshUIDFang),
		MEMBER(MemberType::GAME_OBJECT_UID, playerMeshUIDOnimaru),
		MEMBER(MemberType::GAME_OBJECT_UID, meshUID),
		MEMBER(MemberType::GAME_OBJECT_UID, meshUID1),
		MEMBER(MemberType::GAME_OBJECT_UID, meshUID2),
		MEMBER(MemberType::FLOAT, rangerGruntCharacter.movementSpeed),
		MEMBER(MemberType::INT, rangerGruntCharacter.lifePoints),
		MEMBER(MemberType::FLOAT, rangerGruntCharacter.searchRadius),
		MEMBER(MemberType::FLOAT, rangerGruntCharacter.attackRange),
		MEMBER(MemberType::FLOAT, rangerGruntCharacter.timeToDie),
		MEMBER(MemberType::FLOAT, attackSpeed),
		MEMBER(MemberType::FLOAT, fleeingRange),
		MEMBER(MemberType::PREFAB_RESOURCE_UID, trailPrefabUID),
		MEMBER(MemberType::GAME_OBJECT_UID, dmgMaterialObj),
		MEMBER(MemberType::GAME_OBJECT_UID, hudControllerObjUID),
		MEMBER(MemberType::FLOAT, timeSinceLastHurt),
		MEMBER(MemberType::FLOAT, approachOffset) //This variable should be a positive float, it will be used to make AIs get a bit closer before stopping their approach

};//clang-format on


GENERATE_BODY_IMPL(RangedAI);

void RangedAI::Start() {
	player = GameplaySystems::GetGameObject(playerUID);
	meshObj = GameplaySystems::GetGameObject(meshUID);
	meshObjForFrustumPresenceCheck1 = GameplaySystems::GetGameObject(meshUID1);
	meshObjForFrustumPresenceCheck2 = GameplaySystems::GetGameObject(meshUID2);
	animation = GetOwner().GetComponent<ComponentAnimation>();
	ownerTransform = GetOwner().GetComponent<ComponentTransform>();
	fangMeshObj = GameplaySystems::GetGameObject(playerMeshUIDFang);
	onimaruMeshObj = GameplaySystems::GetGameObject(playerMeshUIDOnimaru);

	shootTrailPrefab = GameplaySystems::GetResource<ResourcePrefab>(trailPrefabUID);


	if (meshObj) {
		ComponentBoundingBox* bb = meshObj->GetComponent<ComponentBoundingBox>();
		bbCenter = (bb->GetLocalMinPointAABB() + bb->GetLocalMaxPointAABB()) / 2;
		meshRenderer = meshObj->GetComponent<ComponentMeshRenderer>();
		if (meshRenderer) {
			noDmgMaterialID = meshRenderer->materialId;
		}
	}
	GameObject* damagedObj = GameplaySystems::GetGameObject(dmgMaterialObj);
	if (damagedObj) {
		ComponentMeshRenderer* dmgMeshRenderer = damagedObj->GetComponent<ComponentMeshRenderer>();
		if (dmgMeshRenderer) {
			damagedMaterialID = dmgMeshRenderer->materialId;
		}
	}


	agent = GetOwner().GetComponent<ComponentAgent>();
	if (agent) {
		agent->SetMaxSpeed(rangerGruntCharacter.movementSpeed);
		agent->SetMaxAcceleration(static_cast<float>(AIMovement::maxAcceleration));
		agent->SetAgentObstacleAvoidance(true);
		agent->RemoveAgentFromCrowd();
	}

	GameObject* hudControllerObj = GameplaySystems::GetGameObject(hudControllerObjUID);
	if (hudControllerObj) {
		hudControllerScript = GET_SCRIPT(hudControllerObj, HUDController);
	}

	if (player) {
		playerController = GET_SCRIPT(player, PlayerController);
	}

	aiMovement = GET_SCRIPT(&GetOwner(), AIMovement);

	// TODO: ADD CHECK PLS
	ChangeState(AIState::START);

	int i = 0;
	for (ComponentAudioSource& src : GetOwner().GetComponents<ComponentAudioSource>()) {
		if (i < static_cast<int>(AudioType::TOTAL)) audios[i] = &src;
		++i;
	}

}

void RangedAI::OnAnimationFinished() {
	if (animation == nullptr) return;

	if (state == AIState::SPAWN) {
		animation->SendTrigger("SpawnIdle");
		state = AIState::IDLE;
	} else if (state == AIState::DEATH) {
		dead = true;
	}
}

void RangedAI::OnAnimationSecondaryFinished() {
	std::string currentStateString = "";
	if (!animation) return;
	if (shot) {
		animation->SendTriggerSecondary("Shoot" + animation->GetCurrentState()->name);
		shot = false;
	}

}

//This is commented until merge with collisions
//
//void RangedAI::OnCollision(const GameObject& collidedWith) {
//	if (state == AIState::START || state != AIState::SPAWN)return;
//	if (rangerGruntCharacter.lifePoints > 0 && playerController) {
//		if (collidedWith.name == "FangBullet") {
//			rangerGruntCharacter.lifePoints -= playerController->fangDamage;
//		} else if (collidedWith.name == "OnimaruBullet") {
//			rangerGruntCharacter.lifePoints -= playerController->onimaruDamage;
//		}
//	}
//
//	if (rangerGruntCharacter.lifePoints <= 0) {
//		ChangeState(AIState::DEATH);
//	}
//
//}

void RangedAI::Update() {

	if (meshRenderer) {
		if (timeSinceLastHurt < hurtFeedbackTimeDuration) {
			timeSinceLastHurt += Time::GetDeltaTime();
			if (timeSinceLastHurt > hurtFeedbackTimeDuration) {
				meshRenderer->materialId = noDmgMaterialID;
			}
		}
	}

	if (!GetOwner().IsActive()) return;

	if (hitTaken && rangerGruntCharacter.lifePoints > 0) {

		if (meshRenderer) {
			if (damagedMaterialID != 0) {
				meshRenderer->materialId = damagedMaterialID;
			}
		}

		rangerGruntCharacter.lifePoints -= rangerGruntCharacter.damageHit;
		hitTaken = false;

		timeSinceLastHurt = 0.0f;

		if (rangerGruntCharacter.lifePoints <= 0 && state != AIState::DEATH) {
			ChangeState(AIState::DEATH);
		}
	}

	if (state == AIState::IDLE || state == AIState::RUN || state == AIState::FLEE) {
		attackTimePool = Max(attackTimePool - Time::GetDeltaTime(), 0.0f);
		if (attackTimePool == 0) {
			if (actualShotTimer == -1) {
				ShootPlayerInRange();
			}
		}
	}

	if (actualShotTimer > 0) {
		actualShotTimer = Max(actualShotTimer - Time::GetDeltaTime(), 0.0f);
		if (actualShotTimer == 0) {
			ActualShot();
		}
	}

	UpdateState();
}

void RangedAI::EnterState(AIState newState) {
	switch (newState) {
	case AIState::START:
		break;
	case AIState::SPAWN:
		PlayAudio(AudioType::SPAWN);
		break;
	case AIState::IDLE:
		if (animation) {
			if (state == AIState::FLEE) {
				animation->SendTrigger("RunBackwardIdle");
			} else if (state == AIState::RUN) {
				animation->SendTrigger("RunForwardIdle");
			}
		}


		if (aiMovement) aiMovement->Stop();
		break;
	case AIState::RUN:
		if (animation) {
			if (state == AIState::IDLE) {
				animation->SendTrigger("IdleRunForward");
			} else if (state == AIState::FLEE) {
				animation->SendTrigger("RunBackwardRunForward");
			}
		}
		break;
	case AIState::FLEE:
		if (animation) {
			if (state == AIState::RUN) {
				animation->SendTrigger("RunForwardRunBackward");
			} else if (state == AIState::IDLE) {
				animation->SendTrigger("IdleRunBackward");
			}
		}
		break;
	case AIState::DEATH:
		if (state == AIState::IDLE) {
			animation->SendTrigger("IdleDeath");
		} else if (state == AIState::RUN) {
			animation->SendTrigger("RunForwardDeath");
		} else if (state == AIState::FLEE) {
			animation->SendTrigger("RunBackwardDeath");
		}

		PlayAudio(AudioType::DEATH);
		agent->RemoveAgentFromCrowd();
		state = AIState::DEATH;
		break;
	}
}

void RangedAI::UpdateState() {
	switch (state) {
	case AIState::START:

		if (Camera::CheckObjectInsideFrustum(GetOwner().GetChildren()[0])) {
			if (aiMovement) aiMovement->Seek(state, float3(ownerTransform->GetGlobalPosition().x, 0, ownerTransform->GetGlobalPosition().z), rangerGruntCharacter.fallingSpeed, true);
			if (ownerTransform->GetGlobalPosition().y < 2.7f + 0e-5f) {
				animation->SendTrigger("StartSpawn");
				ChangeState(AIState::SPAWN);
			}
		}
		break;
	case AIState::SPAWN:
		break;
	case AIState::IDLE:
		if (player) {
			if (aiMovement) {
				if (aiMovement->CharacterInSight(player, rangerGruntCharacter.searchRadius)) {
					if (aiMovement->CharacterInSight(player, fleeingRange)) {
						ChangeState(AIState::FLEE);
						break;
					}

					if (!CharacterInRange(player, rangerGruntCharacter.attackRange, true)) {
						ChangeState(AIState::RUN);
						break;
					}

					if (FindsRayToPlayer(false)) {
						OrientateTo(player->GetComponent<ComponentTransform>()->GetGlobalPosition() - ownerTransform->GetGlobalPosition());
					} else {
						ChangeState(AIState::RUN);
					}
				}
			}
		}
		break;
	case AIState::RUN:
		if (aiMovement) {
			if (aiMovement->CharacterInSight(player, rangerGruntCharacter.searchRadius)) {
				OrientateTo(player->GetComponent<ComponentTransform>()->GetGlobalPosition() - ownerTransform->GetGlobalPosition());

				if (!CharacterInRange(player, rangerGruntCharacter.attackRange - approachOffset, true) || !FindsRayToPlayer(false)) {
					if (!aiMovement->CharacterInSight(player, fleeingRange)) {
						if (aiMovement) aiMovement->Seek(state, player->GetComponent<ComponentTransform>()->GetGlobalPosition(), static_cast<int>(rangerGruntCharacter.movementSpeed), false);
					} else {
						ChangeState(AIState::FLEE);
					}
				} else {
					ChangeState(AIState::IDLE);
				}
			}
		}
		break;
	case AIState::FLEE:
		OrientateTo(player->GetComponent<ComponentTransform>()->GetGlobalPosition() - ownerTransform->GetGlobalPosition());

		if (aiMovement->CharacterInSight(player, fleeingRange)) {
			if (aiMovement) aiMovement->Flee(state, player->GetComponent<ComponentTransform>()->GetGlobalPosition(), static_cast<int>(rangerGruntCharacter.movementSpeed), false);
		} else {
			ChangeState(AIState::IDLE);
		}
		break;
	case AIState::DEATH:
		if (dead) {
			if (dead) {
				if (rangerGruntCharacter.timeToDie > 0) {
					rangerGruntCharacter.timeToDie -= Time::GetDeltaTime();
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

void RangedAI::HitDetected(int damage_) {
	rangerGruntCharacter.damageHit = damage_;
	hitTaken = true;
	PlayAudio(AudioType::HIT);
}

void RangedAI::ChangeState(AIState newState) {
	EnterState(newState);
	state = newState;
}


bool RangedAI::CharacterInRange(const GameObject* character, float range, bool useRange) {

	bool inFrustum0 = meshObj != nullptr ? Camera::CheckObjectInsideFrustum(meshObj) : true;
	bool inFrustum1 = meshObjForFrustumPresenceCheck1 != nullptr ? Camera::CheckObjectInsideFrustum(meshObjForFrustumPresenceCheck1) : true;
	bool inFrustum2 = meshObjForFrustumPresenceCheck2 != nullptr ? Camera::CheckObjectInsideFrustum(meshObjForFrustumPresenceCheck2) : true;
	bool inFrustum = inFrustum0 && inFrustum1 && inFrustum2;
	if (!useRange) {
		return inFrustum;
	}

	ComponentTransform* target = character->GetComponent<ComponentTransform>();
	if (target) {
		float3 posTarget = target->GetGlobalPosition();
		return posTarget.Distance(ownerTransform->GetGlobalPosition()) < range && inFrustum;
	}
	return false;
}

bool RangedAI::FindsRayToPlayer(bool useForward) {
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
	GameObject* hitGo = Physics::Raycast(start, start + dir * rangerGruntCharacter.attackRange, mask);

	return hitGo != nullptr;
}

void RangedAI::OrientateTo(const float3& direction) {
	Quat newRotation = Quat::LookAt(float3(0, 0, 1), direction.Normalized(), float3(0, 1, 0), float3(0, 1, 0));
	ownerTransform->SetGlobalRotation(newRotation);
}

void RangedAI::ActualShot() {
	if (shootTrailPrefab) {
		//TODO WAIT STRETCH FROM LOWY AND IMPLEMENT SOME SHOOT EFFECT
		ComponentBoundingBox* box = meshObj->GetComponent<ComponentBoundingBox>();

		float offsetY = (box->GetWorldAABB().minPoint.y + box->GetWorldAABB().maxPoint.y) / 4;

		GameObject* projectileInstance(GameplaySystems::Instantiate(shootTrailPrefab, GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition() + float3(0, offsetY, 0), Quat(0, 0, 0, 0)));

		if (projectileInstance) {
			RangerProjectileScript* rps = GET_SCRIPT(projectileInstance, RangerProjectileScript);
			if (rps && ownerTransform) {
				rps->SetRangerDirection(ownerTransform->GetGlobalRotation());
			}
		}

		//GameplaySystems::Instantiate(shootTrailPrefab, GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition() + float3(0, offsetY, 0), GetOwner().GetComponent<ComponentTransform>()->GetGlobalRotation());
		//float3 frontTrail = GetOwner().GetComponent<ComponentTransform>()->GetGlobalRotation() * float3(0.0f, 0.0f, 1.0f);
		//GameplaySystems::Instantiate(shootTrailPrefab, GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition() + float3(0, offsetY, 0), Quat::RotateAxisAngle(frontTrail, (pi / 2)).Mul(GetOwner().GetComponent<ComponentTransform>()->GetGlobalRotation()));
	}

	attackTimePool = 1.0f / attackSpeed;
	actualShotTimer = -1.0f;

	PlayAudio(AudioType::SHOOT);


	if (FindsRayToPlayer(true)) {
		Debug::Log("Player was shot");
	}
}

void RangedAI::PlayAudio(AudioType audioType) {
	if (audios[static_cast<int>(audioType)]) audios[static_cast<int>(audioType)]->Play();
}

void RangedAI::ShootPlayerInRange() {
	if (!player) return;
	if (CharacterInRange(player, rangerGruntCharacter.attackRange, true)) {
		shot = true;

		if (animation) {
			animation->SendTriggerSecondary(animation->GetCurrentState()->name + "Shoot");
		}

		actualShotTimer = actualShotMaxTime;
	}
}
