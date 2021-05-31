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

EXPOSE_MEMBERS(PlayerController) {
	// Add members here to expose them to the engine. Example:
	MEMBER(MemberType::GAME_OBJECT_UID, fangUID),
		MEMBER(MemberType::GAME_OBJECT_UID, onimaruUID),
		MEMBER(MemberType::GAME_OBJECT_UID, mainNodeUID),
		MEMBER(MemberType::GAME_OBJECT_UID, cameraUID),
		MEMBER(MemberType::PREFAB_RESOURCE_UID, fangTrailUID),
		MEMBER(MemberType::GAME_OBJECT_UID, fangGunUID),

		MEMBER(MemberType::GAME_OBJECT_UID, onimaruParticleUID),
		MEMBER(MemberType::GAME_OBJECT_UID, switchAudioSourceUID),
		MEMBER(MemberType::GAME_OBJECT_UID, dashAudioSourceUID),
		MEMBER(MemberType::GAME_OBJECT_UID, canvasUID),
		MEMBER(MemberType::FLOAT, distanceRayCast),
		MEMBER(MemberType::FLOAT, switchCooldown),
		MEMBER(MemberType::FLOAT, dashCooldown),
		MEMBER(MemberType::FLOAT, dashSpeed),
		MEMBER(MemberType::FLOAT, dashDistance),
		MEMBER(MemberType::FLOAT, cameraOffsetZ),
		MEMBER(MemberType::FLOAT, cameraOffsetY),
		MEMBER(MemberType::FLOAT, cameraOffsetX),
		MEMBER(MemberType::FLOAT, fangMovementSpeed),
		MEMBER(MemberType::FLOAT, onimaruMovementSpeed),
		MEMBER(MemberType::FLOAT, shootCooldown),
		MEMBER(MemberType::INT, lifePointsFang),
		MEMBER(MemberType::INT, lifePointsOni)
};

GENERATE_BODY_IMPL(PlayerController);

void PlayerController::Start() {
	player = GameplaySystems::GetGameObject(mainNodeUID);
	fang = GameplaySystems::GetGameObject(fangUID);
	onimaru = GameplaySystems::GetGameObject(onimaruUID);
	camera = GameplaySystems::GetGameObject(cameraUID);
	fangGun = GameplaySystems::GetGameObject(fangGunUID);
	GameObject* canvasGO = GameplaySystems::GetGameObject(canvasUID);
	if (canvasGO) {
		hudControllerScript = GET_SCRIPT(canvasGO, HUDController);
	}

	//animation
	//fangParticle = GameplaySystems::GetGameObject(fangParticleUID);
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
		fangAnimation = fang->GetComponent<ComponentAnimation>();
		if (fangAnimation) {
			fangCurrentState = fangAnimation->GetCurrentState();
		}
	}
	if (onimaru) {
		onimaru->Disable();
		onimaruAnimation = onimaru->GetComponent<ComponentAnimation>();
		if (onimaruAnimation) {
			onimaruCurrentState = onimaruAnimation->GetCurrentState();
		}
	}
	//if (fangParticle) {
	//	fangCompParticle = fangParticle->GetComponent<ComponentParticleSystem>();
	//}
	if (onimaruParticle) {
		onimaruCompParticle = onimaruParticle->GetComponent<ComponentParticleSystem>();
	}
	if (switchAudioSourceUID) {
		GameObject* aux = GameplaySystems::GetGameObject(switchAudioSourceUID);
		switchAudioSource = aux->GetComponent<ComponentAudioSource>();
	}
	if (dashAudioSourceUID) {
		GameObject* aux = GameplaySystems::GetGameObject(dashAudioSourceUID);
		dashAudioSource = aux->GetComponent<ComponentAudioSource>();
	}
	firstTime = true;
}

void PlayerController::MoveTo(MovementDirection md) {
	float modifier = 1.0f;
	float3 newPosition = transform->GetGlobalPosition();
	if (Input::GetKeyCode(Input::KEYCODE::KEY_LSHIFT)) {
		modifier = 2.0f;
	}
	float movementSpeed = ((fang->IsActive()) ? fangMovementSpeed : onimaruMovementSpeed);
	newPosition += GetDirection(md) * movementSpeed * Time::GetDeltaTime() * modifier;
	transform->SetGlobalPosition(newPosition);
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
		dashDirection = GetDirection(md);
		dashMovementDirection = md;
		dashDestination = transform->GetGlobalPosition();
		dashDestination += dashDistance * dashDirection;
		dashCooldownRemaing = dashCooldown;
		dashInCooldown = true;
		dashing = true;
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
		newPosition += dashSpeed * Time::GetDeltaTime() * dashDirection;
		transform->SetGlobalPosition(newPosition);
		if (std::abs(std::abs(newPosition.x) - std::abs(dashDestination.x)) < dashError &&
			std::abs(std::abs(newPosition.z) - std::abs(dashDestination.z)) < dashError) {
			dashing = false;
		}
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
			hudControllerScript->UpdateHP(lifePointsOni, lifePointsFang);
		}
		else {
			Debug::Log("Swaping to fang...");
			onimaru->Disable();
			fang->Enable();
			hudControllerScript->UpdateHP(lifePointsFang, lifePointsOni);
		}
		switchCooldownRemaing = switchCooldown;
		if (hudControllerScript) {
			hudControllerScript->ChangePlayerHUD();
		}
	}
}

bool PlayerController::CanShoot() {
	return !shooting && ((fang && fangCompParticle) || (onimaru && onimaruCompParticle));
}

void PlayerController::Shoot() {
	ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
	FangGuntransform = fangGun->GetComponent<ComponentTransform>();

	if (CanShoot()) {
		/*if (shootAudioSource) {
			shootAudioSource->Play();
		}
		else {
			Debug::Log(AUDIOSOURCE_NULL_MSG);
		}*/

		shootCooldownRemaing = shootCooldown;
		shooting = true;
		if (fang->IsActive()) {
			ResourcePrefab* prefab = GameplaySystems::GetResource<ResourcePrefab>(fangTrailUID);
			if (prefab != nullptr) {
				//fangGun->GetComponent<ComponentParticleSystem>()->Play();
				GameplaySystems::Instantiate(prefab, FangGuntransform->GetGlobalPosition(), transform->GetGlobalRotation());
				float3 frontTrail = transform->GetGlobalRotation() * float3(0.0f, 0.0f, 1.0f);
				GameObject* secondTrail = GameplaySystems::Instantiate(prefab, FangGuntransform->GetGlobalPosition(), Quat::RotateAxisAngle(frontTrail, (pi / 2)).Mul(transform->GetGlobalRotation()));
				/*fangCompParticle->Play();*/
			}
			else {
				onimaruCompParticle->Play();
			}

			float3 start = transform->GetGlobalPosition(); //(boundingBox->GetLocalMaxPointAABB() + boundingBox->GetLocalMinPointAABB()) / 2;
			float3 end = transform->GetGlobalRotation() * float3(0, 0, 1);
			end.Normalize();
			end *= distanceRayCast;
			int mask = static_cast<int>(MaskType::ENEMY);
			GameObject* hitGo = Physics::Raycast(start, start + end, mask);
			if (hitGo) {
				AIMovement* enemyScript = GET_SCRIPT(hitGo, AIMovement);
				if (fang->IsActive()) enemyScript->HitDetected(3);
				else enemyScript->HitDetected();
			}
		}
	}
}

void PlayerController::HitDetected() {
	hitTaken = true;
}

void PlayerController::CheckCoolDowns() {
	if (dashCooldownRemaing <= 0.f) {
		dashCooldownRemaing = 0.f;
		dashInCooldown = false;
		dashMovementDirection = MovementDirection::NONE;
	}
	else {
		dashCooldownRemaing -= Time::GetDeltaTime();
	}

	if (switchCooldownRemaing <= 0.f) {
		switchCooldownRemaing = 0.f;
		switchInCooldown = false;
	}
	else {
		switchCooldownRemaing -= Time::GetDeltaTime();
	}

	if (shootCooldownRemaing <= 0.f) {
		shootCooldownRemaing = 0.f;
		shooting = false;
	}
	else {
		shootCooldownRemaing -= Time::GetDeltaTime();
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
	float3 direction = float3(0, 0, 0);
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
		break;
	}
	return direction;
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

	switch (md) {
	case MovementDirection::NONE:
		animation->SendTrigger(currentState->name + PlayerController::states[0]);
		break;
	case MovementDirection::LEFT:
		animation->SendTrigger(currentState->name + PlayerController::states[GetMouseDirectionState(md) + dashAnimation]);
		break;
	case MovementDirection::RIGHT:
		animation->SendTrigger(currentState->name + PlayerController::states[GetMouseDirectionState(md) + dashAnimation]);
		break;
	case MovementDirection::UP:
		animation->SendTrigger(currentState->name + PlayerController::states[GetMouseDirectionState(md) + dashAnimation]);
		break;
	case MovementDirection::DOWN:
		animation->SendTrigger(currentState->name + PlayerController::states[GetMouseDirectionState(md) + dashAnimation]);
		break;
	}
}

void PlayerController::UpdatePlayerStats() {
	if (hudControllerScript) {
		if (firstTime) {
			hudControllerScript->UpdateHP(lifePointsFang, lifePointsOni);
			firstTime = false;
		}

		if (hitTaken && fang->IsActive() && lifePointsFang > 0) {
			--lifePointsFang;
			hudControllerScript->UpdateHP(lifePointsFang, lifePointsOni);
			hitTaken = false;
		}
		else if (hitTaken && onimaru->IsActive() && lifePointsOni > 0) {
			--lifePointsOni;
			hudControllerScript->UpdateHP(lifePointsOni, lifePointsFang);
			hitTaken = false;
		}

		float realDashCooldown = 1.0f - (dashCooldownRemaing / dashCooldown);
		float realSwitchCooldown = 1.0f - (switchCooldownRemaing / switchCooldown);
		hudControllerScript->UpdateCooldowns(0.0f, 0.0f, 0.0f, realDashCooldown, 0.0f, 0.0f, realSwitchCooldown);

		if (lifePointsFang <= 0 || lifePointsOni <= 0) {
			SceneManager::ChangeScene("Assets/Scenes/LoseScene.scene");
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
	Dash();
	UpdatePlayerStats();
	UpdateCameraPosition();
	if (firstTime) {
		if (fang->IsActive()) {
			hudControllerScript->UpdateHP(lifePointsFang, lifePointsOni);
		}
		else {
			hudControllerScript->UpdateHP(lifePointsOni, lifePointsFang);
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
}