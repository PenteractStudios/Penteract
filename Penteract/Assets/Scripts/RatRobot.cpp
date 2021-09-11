#include "RatRobot.h"

#include "GameplaySystems.h"
#include "AIMovement.h"
#include "GameController.h"

EXPOSE_MEMBERS(RatRobot) {
	MEMBER(MemberType::GAME_OBJECT_UID, playerId),
	MEMBER(MemberType::FLOAT, fleeRange)
};

GENERATE_BODY_IMPL(RatRobot);

void RatRobot::Start() {
	player = GameplaySystems::GetGameObject(playerId);
	if (!player) player = GameplaySystems::GetGameObject("Player");
	if (player) playerTransform = player->GetComponent<ComponentTransform>();

	transform = GetOwner().GetComponent<ComponentTransform>();
	agent = GetOwner().GetComponent<ComponentAgent>();
	animation = GetOwner().GetComponent<ComponentAnimation>();

	aiMovement = GET_SCRIPT(&GetOwner(), AIMovement);
}

void RatRobot::Update() {
	switch (state) {
	case RatRobotState::IDLE1:
		if (!aiMovement) break;

		aiMovement->Stop();

		if (PlayerInRange()) {
			ChangeState(RatRobotState::RUN);
		}
		break;
	case RatRobotState::RUN:
		if (!transform || !agent) break;

		if (PlayerInRange()) {
			float3 position = transform->GetGlobalPosition();
			float3 playerPosition = playerTransform->GetGlobalPosition();
			float3 direction = (position - playerPosition).Normalized();
			float3 fleeDestination = playerPosition + direction * fleeRange;

			agent->SetMoveTarget(fleeDestination, true);
			float3 velocity = agent->GetVelocity();
			Quat targetRotation = Quat::LookAt(float3(-1, 0, 0), velocity.Normalized(), float3(0, 1, 0), float3(0, 1, 0));
			transform->SetGlobalRotation(targetRotation);
		}
		else {
			ChangeState(RatRobotState::IDLE1);
		}
		break;
	}
}

void RatRobot::ChangeState(RatRobotState newState) {
	if (!agent || !animation) return;

	switch (newState) {
	case RatRobotState::IDLE1:
		switch (state) {
		case RatRobotState::IDLE2:
			animation->SendTrigger("Idle2Idle1");
			break;
		case RatRobotState::IDLE3:
			animation->SendTrigger("Idle3Idle1");
			break;
		case RatRobotState::RUN:
			animation->SendTrigger("RunIdle1");
			break;
		}

		if (aiMovement) aiMovement->Stop();
		break;
	case RatRobotState::IDLE2:
		switch (state) {
		case RatRobotState::IDLE1:
			animation->SendTrigger("Idle1Idle2");
			break;
		case RatRobotState::IDLE3:
			animation->SendTrigger("Idle3Idle2");
			break;
		case RatRobotState::RUN:
			animation->SendTrigger("RunIdle2");
			break;
		}

		if (aiMovement) aiMovement->Stop();
		break;
	case RatRobotState::IDLE3:
		switch (state) {
		case RatRobotState::IDLE1:
			animation->SendTrigger("Idle1Idle3");
			break;
		case RatRobotState::IDLE2:
			animation->SendTrigger("Idle2Idle3");
			break;
		case RatRobotState::RUN:
			animation->SendTrigger("RunIdle3");
			break;
		}

		if (aiMovement) aiMovement->Stop();
		break;
	case RatRobotState::RUN:
		switch (state) {
		case RatRobotState::IDLE1:
			animation->SendTrigger("Idle1Run");
			break;
		case RatRobotState::IDLE2:
			animation->SendTrigger("Idle2Run");
			break;
		case RatRobotState::IDLE3:
			animation->SendTrigger("Idle3Run");
			break;
		}
		break;
	}

	state = newState;
}

bool RatRobot::PlayerInRange() {
	if (!transform || !playerTransform) return false;

	return playerTransform->GetGlobalPosition().DistanceSq(transform->GetGlobalPosition()) < fleeRange * fleeRange;
}