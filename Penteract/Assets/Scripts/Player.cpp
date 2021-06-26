#include "Player.h"
#include "Geometry/Plane.h"
#include "Geometry/Frustum.h"
#include "Geometry/LineSegment.h"

void Player::SetAttackSpeed(float attackSpeed_)
{
	attackSpeed = attackSpeed_;
}

void Player::Hit(float damage_)
{
	lifePoints -= damage_;
	if (playerAudios[static_cast<int>(AudioPlayer::HIT)]) playerAudios[static_cast<int>(AudioPlayer::HIT)]->Play();
	isAlive = lifePoints > 0.0f;
	if (!isAlive) {
		if (playerAudios[static_cast<int>(AudioPlayer::DEATH)]) playerAudios[static_cast<int>(AudioPlayer::DEATH)]->Play();
	}
}

void Player::ResetSwitchStatus() {
	switchInProgress = false;
	playSwitchParticles = true;
	currentSwitchDelay = 0.f;
}

void Player::MoveTo() {
	float3 newPosition = playerMainTransform->GetGlobalPosition() + GetDirection();
	agent->SetMaxSpeed(movementSpeed);
	agent->SetMoveTarget(newPosition, false);
}

bool Player::CanShoot()
{
	return !shooting;
}

MovementDirection Player::GetInputMovementDirection() const {
	MovementDirection md = MovementDirection::NONE;
	if (Input::GetKeyCode(Input::KEYCODE::KEY_W)) {
		md = MovementDirection::UP;
	}

	if (Input::GetKeyCode(Input::KEYCODE::KEY_S)) {
		md = MovementDirection::DOWN;
	}

	if (Input::GetKeyCode(Input::KEYCODE::KEY_A)) {
		if (md == MovementDirection::UP) md = MovementDirection::UP_LEFT;
		else if (md == MovementDirection::DOWN) md = MovementDirection::DOWN_LEFT;
		else md = MovementDirection::LEFT;
	}

	if (Input::GetKeyCode(Input::KEYCODE::KEY_D)) {
		if (md == MovementDirection::UP) md = MovementDirection::UP_RIGHT;
		else if (md == MovementDirection::DOWN) md = MovementDirection::DOWN_RIGHT;
		else md = MovementDirection::RIGHT;
	}
	return md;
}

int Player::GetMouseDirectionState() {
	float3 inputDirection = GetDirection();
	float dot = Dot(inputDirection.Normalized(), facePointDir.Normalized());
	float3 cross = Cross(inputDirection.Normalized(), facePointDir.Normalized());

	if (dot > 0.707) {
		return 2; //RunForward
	}
	else if (dot < -0.707) {
		return 1; //RunBackward
	}
	else if (cross.y > 0) {
		return 4; //RunRight
	}
	else {
		return 3; //RunLeft
	}
}

bool Player::IsActive()
{
	return (characterGameObject) ? characterGameObject->IsActive() : false;
}

float3 Player::GetDirection() const {
	float3 direction;
	switch (movementInputDirection) {
	case MovementDirection::UP:
		direction = float3(0, 0, -1);
		break;
	case MovementDirection::UP_LEFT:
		direction = float3(-0.5, 0, -0.5);
		break;
	case MovementDirection::UP_RIGHT:
		direction = float3(0.5, 0, -0.5);
		break;
	case MovementDirection::DOWN:
		direction = float3(0, 0, 1);
		break;
	case MovementDirection::DOWN_LEFT:
		direction = float3(-0.5, 0, 0.5);
		break;
	case MovementDirection::DOWN_RIGHT:
		direction = float3(0.5, 0, 0.5);
		break;
	case MovementDirection::RIGHT:
		direction = float3(1, 0, 0);
		break;
	case MovementDirection::LEFT:
		direction = float3(-1, 0, 0);
		break;
	default:
		return float3(0, 0, 0);
	}
	return direction.Normalized();
}

void Player::LookAtMouse() {
	if (lookAtMouseCameraComp) {
		float2 mousePos = Input::GetMousePositionNormalized();
		LineSegment ray = lookAtMouseCameraComp->frustum.UnProjectLineSegment(mousePos.x, mousePos.y);
		float3 planeTransform = lookAtMousePlanePosition;
		Plane p = Plane(planeTransform, float3(0, 1, 0));
		facePointDir = float3(0, 0, 0);
		facePointDir = p.ClosestPoint(ray) - (playerMainTransform->GetGlobalPosition());
		Quat quat = playerMainTransform->GetGlobalRotation();
		float angle = Atan2(facePointDir.x, facePointDir.z);
		Quat rotation = quat.RotateAxisAngle(float3(0, 1, 0), angle);
		playerMainTransform->SetGlobalRotation(rotation);
	}
}

void Player::Update(bool lockMovement) {
	if(!lockMovement){
		movementInputDirection = GetInputMovementDirection();
		MoveTo();
		LookAtMouse();
	}
}

