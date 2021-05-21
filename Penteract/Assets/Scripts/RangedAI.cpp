#include "RangedAI.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "PlayerController.h"
#include "Components/ComponentTransform.h"


EXPOSE_MEMBERS(RangedAI) {
	MEMBER(MemberType::GAME_OBJECT_UID, playerUID),
		MEMBER(MemberType::INT, maxMovementSpeed),
		MEMBER(MemberType::INT, lifePoints),
		MEMBER(MemberType::FLOAT, searchRadius),
		MEMBER(MemberType::FLOAT, attackRange),
		MEMBER(MemberType::FLOAT, timeToDie),
		MEMBER(MemberType::FLOAT, attackSpeed),
		MEMBER(MemberType::FLOAT, fleeingRange)
};

GENERATE_BODY_IMPL(RangedAI);

void RangedAI::Start() {
	player = GameplaySystems::GetGameObject(playerUID);
	animation = GetOwner().GetParent()->GetComponent<ComponentAnimation>();
	parentTransform = GetOwner().GetParent()->GetComponent<ComponentTransform>();
}

void RangedAI::ReceiveEvent(TesseractEvent& e) {
	switch (e.type) {
	case TesseractEventType::ANIMATION_FINISHED:

		if (state == AIState::SPAWN) {
			animation->SendTrigger("SpawnIdle");
			state = AIState::IDLE;
		} else if (state == AIState::HURT && lifePoints > 0) {
			animation->SendTrigger("HurtIdle");
			state = AIState::IDLE;
		}

		else if (state == AIState::HURT && lifePoints <= 0) {
			//animation->SendTrigger("HurtDeath");
			Debug::Log("Death");
			state = AIState::DEATH;
		} else if (state == AIState::DEATH) {
			dead = true;
		}
		break;
	}
}

void RangedAI::Update() {
	if (!GetOwner().IsActive()) return;

	if (hitTaken && lifePoints > 0) {
		if (state == AIState::IDLE || state == AIState::HURT) {
			animation->SendTrigger("IdleHurt");
		} else if (state == AIState::RUN) {
			animation->SendTrigger("RunHurt");
		}

		lifePoints -= damageRecieved;
		state = AIState::HURT;
		hitTaken = false;
	}

	if (state != AIState::DEATH && state != AIState::HURT) {
		attackTimePool = Max(attackTimePool - Time::GetDeltaTime(), 0.0f);
		if (attackTimePool == 0) ShootPlayerInRange();
	}

	switch (state) {
	case AIState::START:
		state = AIState::IDLE;
		break;
	case AIState::SPAWN:
		state = AIState::IDLE;
		break;
	case AIState::IDLE:
		if (player) {
			if (CharacterInSight(player)) {
				animation->SendTrigger("IdleRun");
				state = AIState::RUN;
			}
		}
		break;
	case AIState::RUN:

		if (CharacterInSight(player)) {
			if (!CharacterInRange(player)) {
				Seek(player->GetComponent<ComponentTransform>()->GetGlobalPosition(), maxMovementSpeed);
			} else if (CharacterTooClose(player)) {
				Flee(player->GetComponent<ComponentTransform>()->GetGlobalPosition(), maxMovementSpeed);
			}
		} else {
			state = AIState::IDLE;
		}

		break;
	case AIState::HURT:
		if (timeStunned > maxStunnedTime) {
			state = AIState::IDLE;
			timeStunned = 0;
		} else {
			timeStunned += Time::GetDeltaTime();
		}
		break;
	case AIState::DEATH:
		break;
	default:
		break;
	}
}


void RangedAI::HitDetected(int damage_) {
	damageRecieved = damage_;
	hitTaken = true;
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
	ComponentTransform* target = character->GetComponent<ComponentTransform>();
	if (target) {
		float3 posTarget = target->GetGlobalPosition();
		return posTarget.Distance(parentTransform->GetGlobalPosition()) < attackRange;
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

	float3 position = parentTransform->GetGlobalPosition();
	float3 direction = newPosition - position;

	velocity = direction.Normalized() * speed;

	position += velocity * Time::GetDeltaTime();

	parentTransform->SetGlobalPosition(position);

	if (state != AIState::START) {
		Quat newRotation = Quat::LookAt(float3(0, 0, 1), direction.Normalized(), float3(0, 1, 0), float3(0, 1, 0));
		parentTransform->SetGlobalRotation(newRotation);
	}
}

void RangedAI::Flee(const float3& fromPosition, int speed) {
	float3 position = parentTransform->GetGlobalPosition();
	float3 direction = position - fromPosition;

	velocity = direction.Normalized() * speed;

	position += velocity * Time::GetDeltaTime();

	parentTransform->SetGlobalPosition(position);

	if (state != AIState::START) {
		Quat newRotation = Quat::LookAt(float3(0, 0, 1), direction.Normalized(), float3(0, 1, 0), float3(0, 1, 0));
		parentTransform->SetGlobalRotation(newRotation);
	}
}

void RangedAI::ShootPlayerInRange() {
	if (!player) return;
	if (CharacterInRange(player)) {
		Debug::Log("Shoot");
		attackTimePool = 1.0f / attackSpeed;
	}
}
