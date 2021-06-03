#include "PlayerController.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "TesseractEvent.h"

#include "AIMovement.h"
#include "HUDController.h"

#include "Math/Quat.h"
#include "Geometry/Plane.h"
#include "Geometry/Frustum.h"
#include "Geometry/LineSegment.h"
#include "Math/float3x3.h"
#include "Math/float2.h"
#include <algorithm>
#include <string>

#define PI 3.14159
#define AUDIOSOURCE_NULL_MSG "shootAudioSource is NULL"
#define MAX_ACCELERATION 9999

EXPOSE_MEMBERS(PlayerController) {
	// Add members here to expose them to the engine. Example:
	MEMBER(MemberType::GAME_OBJECT_UID, fangUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruUID),
	MEMBER(MemberType::GAME_OBJECT_UID, mainNodeUID),
	MEMBER(MemberType::GAME_OBJECT_UID, cameraUID),
	MEMBER(MemberType::PREFAB_RESOURCE_UID, fangTrailUID),
	MEMBER(MemberType::PREFAB_RESOURCE_UID, onimaruTrailUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fangGunUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruGunUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruParticleUID),
	MEMBER(MemberType::GAME_OBJECT_UID, switchAudioSourceUID),
	MEMBER(MemberType::GAME_OBJECT_UID, dashAudioSourceUID),
	MEMBER(MemberType::GAME_OBJECT_UID, canvasUID),
	MEMBER(MemberType::FLOAT, distanceRayCast),
	MEMBER(MemberType::FLOAT, switchCooldown),
	MEMBER(MemberType::FLOAT, dashCooldown),
	MEMBER(MemberType::FLOAT, dashSpeed),
	MEMBER(MemberType::FLOAT, dashDuration),
	MEMBER(MemberType::FLOAT, cameraOffsetZ),
	MEMBER(MemberType::FLOAT, cameraOffsetY),
	MEMBER(MemberType::FLOAT, cameraOffsetX),
	MEMBER(MemberType::INT, fangCharacter.lifePoints),
	MEMBER(MemberType::FLOAT, fangCharacter.movementSpeed),
	MEMBER(MemberType::INT, fangCharacter.damageHit),
	MEMBER(MemberType::FLOAT, fangCharacter.shootCooldown),
	MEMBER(MemberType::FLOAT, fangCharacter.attackSpeed),
	MEMBER(MemberType::INT, onimaruCharacter.lifePoints),
	MEMBER(MemberType::FLOAT, onimaruCharacter.movementSpeed),
	MEMBER(MemberType::INT, onimaruCharacter.damageHit),
	MEMBER(MemberType::FLOAT, onimaruCharacter.shootCooldown),
	MEMBER(MemberType::FLOAT, onimaruCharacter.attackSpeed),
};

GENERATE_BODY_IMPL(PlayerController);

void PlayerController::Start() {
	player = GameplaySystems::GetGameObject(mainNodeUID);
	fang = GameplaySystems::GetGameObject(fangUID);
	onimaru = GameplaySystems::GetGameObject(onimaruUID);
	camera = GameplaySystems::GetGameObject(cameraUID);
	GameObject* canvasGO = GameplaySystems::GetGameObject(canvasUID);
	if (canvasGO) {
		hudControllerScript = GET_SCRIPT(canvasGO, HUDController);
	}

	//animation
	onimaruParticle = GameplaySystems::GetGameObject(onimaruParticleUID);

	if (player) {
		transform = player->GetComponent<ComponentTransform>();
		if (transform) {
			initialPosition = transform->GetGlobalPosition();
		}
		shootAudioSource = player->GetComponent<ComponentAudioSource>();
	}
	if (camera) {
		compCamera = camera->GetComponent<ComponentCamera>();
		if (compCamera) {
			GameplaySystems::SetRenderCamera(compCamera);
		}
		cameraTransform = camera->GetComponent<ComponentTransform>();
	}
	if (fang) {
		fang->Enable();
		fangGun = GameplaySystems::GetGameObject(fangGunUID);
		if (fangGun) {
			fangGunTransform = fangGun->GetComponent<ComponentTransform>();
		}
		fangAnimation = fang->GetComponent<ComponentAnimation>();
		fangTrail = GameplaySystems::GetResource<ResourcePrefab>(fangTrailUID);
		if (fangAnimation) {
			fangCurrentState = fangAnimation->GetCurrentState();
		}
	}
	if (onimaru) {
		onimaru->Disable();
		onimaruGun = GameplaySystems::GetGameObject(fangGunUID);
		if (onimaruGun) {
			onimaruGunTransform = onimaruGun->GetComponent<ComponentTransform>();
		}
		onimaruAnimation = onimaru->GetComponent<ComponentAnimation>();
		onimaruTrail = GameplaySystems::GetResource<ResourcePrefab>(onimaruTrailUID);
		if (onimaruAnimation) {
			onimaruCurrentState = onimaruAnimation->GetCurrentState();
		}
	}
	if (onimaruParticle) {
		onimaruCompParticle = onimaruParticle->GetComponent<ComponentParticleSystem>();
	}
	GameObject* aux = GameplaySystems::GetGameObject(switchAudioSourceUID);
	if (aux) {
		switchAudioSource = aux->GetComponent<ComponentAudioSource>();
	}
	aux = GameplaySystems::GetGameObject(dashAudioSourceUID);
	if (aux) {
		dashAudioSource = aux->GetComponent<ComponentAudioSource>();
	}
	firstTime = true;

	agent = GetOwner().GetComponent<ComponentAgent>();
	if (agent) {
		agent->SetMaxSpeed(fangCharacter.movementSpeed);
		agent->SetMaxAcceleration(MAX_ACCELERATION);
	}
}

void PlayerController::MoveTo(MovementDirection md) {
	float modifier = 1.0f;
	float3 newPosition = transform->GetGlobalPosition();
	if (Input::GetKeyCode(Input::KEYCODE::KEY_LSHIFT)) {
		modifier = 2.0f;
	}

	float movementSpeed = ((fang->IsActive()) ? fangCharacter.movementSpeed : onimaruCharacter.movementSpeed);

	//with navigation
	newPosition += GetDirection(md) * movementSpeed * modifier;
	agent->SetMoveTarget(newPosition, false);

}

void PlayerController::LookAtMouse() {
	if (camera && compCamera) {
		float2 mousePos = Input::GetMousePositionNormalized();
		LineSegment ray = compCamera->frustum.UnProjectLineSegment(mousePos.x, mousePos.y);
		Plane p = Plane(transform->GetGlobalPosition(), float3(0, 1, 0));
		facePointDir = float3(0, 0, 0);
		facePointDir = p.ClosestPoint(ray) - (transform->GetGlobalPosition());
		Quat quat = transform->GetRotation();
		float angle = Atan2(facePointDir.x, facePointDir.z);
		Quat rotation = quat.RotateAxisAngle(float3(0, 1, 0), angle);
		transform->SetRotation(rotation);
	}
}

void PlayerController::InitDash(MovementDirection md) {
	if (CanDash()) {
		if (md != MovementDirection::NONE) {
			dashDirection = GetDirection(md);
			dashMovementDirection = md;
		}
		else {
			dashDirection = facePointDir;
		}
		dashCooldownRemaining = dashCooldown;
		dashRemaining = dashDuration;
		dashInCooldown = true;
		dashing = true;
		agent->SetMaxSpeed(dashSpeed);
		if (shootAudioSource) {
			dashAudioSource->Play();
		}
		else {
			Debug::Log(AUDIOSOURCE_NULL_MSG);
		}
	}
}

void PlayerController::Dash() {
	if (dashing) {
		float3 newPosition = transform->GetGlobalPosition();
		newPosition += dashSpeed * dashDirection;
		agent->SetMoveTarget(newPosition, false);

	}
}

bool PlayerController::CanDash() {
	return !dashing && !dashInCooldown && fang->IsActive();
}

bool PlayerController::CanSwitch() {
	return !switchInCooldown;
}

void PlayerController::SwitchCharacter() {
	if (!fang) return;
	if (!onimaru) return;
	if (CanSwitch()) {
		switchInCooldown = true;
		switchAudioSource->Play();
		if (fang->IsActive()) {
			Debug::Log("Swaping to onimaru...");
			fang->Disable();
			onimaru->Enable();
			hudControllerScript->UpdateHP(onimaruCharacter.lifePoints, fangCharacter.lifePoints);
		}
		else {
			Debug::Log("Swaping to fang...");
			onimaru->Disable();
			fang->Enable();
			hudControllerScript->UpdateHP(fangCharacter.lifePoints, onimaruCharacter.lifePoints);
		}
		switchCooldownRemaining = switchCooldown;
		if (hudControllerScript) {
			hudControllerScript->ChangePlayerHUD();
		}
	}
}

bool PlayerController::CanShoot() {

	return !shooting && ((fang->IsActive() && fangTrail) || (onimaru->IsActive() && onimaruTrail));
}

void PlayerController::Shoot() {
	ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();

	if (CanShoot()) {
		if (shootAudioSource) {
			shootAudioSource->Play();
		}
		else {
			Debug::Log(AUDIOSOURCE_NULL_MSG);
		}

		shooting = true;
		float3 start;
		if (fang->IsActive()) {
			fangAttackCooldownRemaining = 1.f / fangCharacter.attackSpeed;
			if (fangTrail) {
				//TODO WAIT STRETCH FROM LOWY AND IMPLEMENT SOME SHOOT EFFECT
				//fangGun->GetComponent<ComponentParticleSystem>()->Play();
				GameplaySystems::Instantiate(fangTrail, fangGunTransform->GetGlobalPosition(), transform->GetGlobalRotation());
				float3 frontTrail = transform->GetGlobalRotation() * float3(0.0f, 0.0f, 1.0f);
				GameplaySystems::Instantiate(fangTrail, fangGunTransform->GetGlobalPosition(), Quat::RotateAxisAngle(frontTrail, (pi / 2)).Mul(transform->GetGlobalRotation()));
			}
			start = fangGunTransform->GetGlobalPosition();
		}
		else {
			//TODO: SUB WITH ONIMARU SHOOT
			onimaruAttackCooldownRemaining = 1.f / onimaruCharacter.attackSpeed;
			if (onimaruTrail) {
				GameplaySystems::Instantiate(onimaruTrail, onimaruGunTransform->GetGlobalPosition(), transform->GetGlobalRotation());
				float3 frontTrail = transform->GetGlobalRotation() * float3(0.0f, 0.0f, 1.0f);
				GameplaySystems::Instantiate(onimaruTrail, onimaruGunTransform->GetGlobalPosition(), Quat::RotateAxisAngle(frontTrail, (pi / 2)).Mul(transform->GetGlobalRotation()));
			}
			start = onimaruGunTransform->GetGlobalPosition();
		}

		float3 end = transform->GetGlobalRotation() * float3(0, 0, 1);
		end.Normalize();
		end *= distanceRayCast;
		int mask = static_cast<int>(MaskType::ENEMY);
		GameObject* hitGo = Physics::Raycast(start, start + end, mask);
		if (hitGo) {
			AIMovement* enemyScript = GET_SCRIPT(hitGo->GetParent(), AIMovement);
			if (fang->IsActive()) enemyScript->HitDetected(fangCharacter.damageHit);
			else enemyScript->HitDetected(onimaruCharacter.damageHit);
		}
	}
}

void PlayerController::HitDetected(int damage) {
	if (fang->IsActive()) {
		fangCharacter.Hit(damage);
	} else {
		onimaruCharacter.Hit(damage);
	}
	hitTaken = true;
}

bool PlayerController::IsDead(){
	return (!fangCharacter.isAlive || !onimaruCharacter.isAlive);
}

void PlayerController::CheckCoolDowns() {

	if (switchCooldownRemaining <= 0.f) {
		switchCooldownRemaining = 0.f;
		switchInCooldown = false;
	}
	else {
		switchCooldownRemaining -= Time::GetDeltaTime();
	}

	if (fang->IsActive()) {
		//Dash Cooldown
		if (dashCooldownRemaining <= 0.f) {
			dashCooldownRemaining = 0.f;
			dashInCooldown = false;
			dashMovementDirection = MovementDirection::NONE;
		}
		else {
			dashCooldownRemaining -= Time::GetDeltaTime();
		}
		//Dash duration
		if (dashRemaining <= 0) {
			dashRemaining = 0.f;
			dashing = false;
			agent->SetMaxSpeed(fangCharacter.movementSpeed);
		}
		else {
			dashRemaining -= Time::GetDeltaTime();
		}

		if (fangAttackCooldownRemaining <= 0) {
			fangAttackCooldownRemaining = 0.f;
			shooting = false;
		}
		else {
			fangAttackCooldownRemaining -= Time::GetDeltaTime();
		}
	}
	if (onimaru->IsActive()) {
		if (onimaruAttackCooldownRemaining <= 0) {
			onimaruAttackCooldownRemaining = 0.f;
			shooting = false;
		}
		else {
			onimaruAttackCooldownRemaining -= Time::GetDeltaTime();
		}
	}
}

MovementDirection PlayerController::GetInputMovementDirection() const {
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

float3 PlayerController::GetDirection(MovementDirection md) const {
	float3 direction;
	switch (md)
	{
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

int PlayerController::GetMouseDirectionState(MovementDirection input) {
	float3 inputDirection = GetDirection(input);
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

void PlayerController::PlayAnimation(MovementDirection md) {
	ComponentAnimation* animation = nullptr;
	State* currentState = nullptr;

	if (fang->IsActive()) {
		animation = fangAnimation;
		currentState = fangCurrentState;
	}
	else {
		animation = onimaruAnimation;
		currentState = onimaruCurrentState;
	}

	if (currentState != animation->GetCurrentState()) {
		currentState = animation->GetCurrentState();
	}

	int dashAnimation = 0;
	if (dashing) {
		dashAnimation = 4;
		md = dashMovementDirection;
	}

	if (md == MovementDirection::NONE) {
		if (IsDead()) {
			animation->SendTrigger(currentState->name + PlayerController::states[9]);
		}
		else {
			animation->SendTrigger(currentState->name + PlayerController::states[0]);
		}
	}
	else {
		animation->SendTrigger(currentState->name + PlayerController::states[GetMouseDirectionState(md) + dashAnimation]);
	}


}

void PlayerController::UpdatePlayerStats() {
	if (hudControllerScript) {
		if (firstTime) {
			hudControllerScript->UpdateHP(fangCharacter.lifePoints, onimaruCharacter.lifePoints);
			firstTime = false;
		}

		if (hitTaken && fang->IsActive() && fangCharacter.lifePoints > 0) {
			--fangCharacter.lifePoints;
			hudControllerScript->UpdateHP(fangCharacter.lifePoints, onimaruCharacter.lifePoints);
			hitTaken = false;
		}
		else if (hitTaken && onimaru->IsActive() && onimaruCharacter.lifePoints > 0) {
			--onimaruCharacter.lifePoints;
			hudControllerScript->UpdateHP(onimaruCharacter.lifePoints, fangCharacter.lifePoints);
			hitTaken = false;
		}

		float realDashCooldown = 1.0f - (dashCooldownRemaining / dashCooldown);
		float realSwitchCooldown = 1.0f - (switchCooldownRemaining / switchCooldown);
		hudControllerScript->UpdateCooldowns(0.0f, 0.0f, 0.0f, realDashCooldown, 0.0f, 0.0f, realSwitchCooldown);

		if (IsDead()) {
			PlayAnimation(MovementDirection::NONE);
		}
	}
}

void PlayerController::UpdateCameraPosition() {
	float3 playerGlobalPos = transform->GetGlobalPosition();
	cameraTransform->SetGlobalPosition(float3(
		playerGlobalPos.x + cameraOffsetX,
		playerGlobalPos.y + cameraOffsetY,
		playerGlobalPos.z + cameraOffsetZ));
}

void PlayerController::Update() {
	if (!player) return;
	if (!camera) return;
	if (!transform) return;

	CheckCoolDowns();
	UpdatePlayerStats();

	if (!IsDead()) {
		Dash();
		UpdateCameraPosition();

		if (firstTime) {
			if (fang->IsActive()) {
				hudControllerScript->UpdateHP(fangCharacter.lifePoints, onimaruCharacter.lifePoints);
			}
			else {
				hudControllerScript->UpdateHP(onimaruCharacter.lifePoints, fangCharacter.lifePoints);
			}
			firstTime = false;
		}

		MovementDirection md;
		md = GetInputMovementDirection();
		if (Input::GetMouseButtonDown(2)) {
			InitDash(md);
		}
		if (!dashing) {
			LookAtMouse();
			MoveTo(md);
			if (Input::GetKeyCode(Input::KEYCODE::KEY_R)) SwitchCharacter();
		}
		if (fang->IsActive()) {
			if (Input::GetMouseButtonDown(0)) Shoot();
		}
		else {
			if (Input::GetMouseButtonRepeat(0)) Shoot();
		}
		PlayAnimation(md);
	} else {
		agent->RemoveAgentFromCrowd();
	}
}
