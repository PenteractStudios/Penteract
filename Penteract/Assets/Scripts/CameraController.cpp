#include "CameraController.h"

#include "PlayerController.h"
#include "Components/ComponentTransform.h"
#include "GlobalVariables.h"

#include "Geometry/LineSegment.h"
#include "Geometry/Plane.h"

#define PI 3.14159f

EXPOSE_MEMBERS(CameraController) {
	MEMBER(MemberType::GAME_OBJECT_UID, playerControllerObjUID),
	MEMBER_SEPARATOR("Camera Positioning"),
	MEMBER(MemberType::FLOAT, cameraOffsetX),
	MEMBER(MemberType::FLOAT, cameraOffsetY),
	MEMBER(MemberType::FLOAT, cameraOffsetZ),
	MEMBER(MemberType::FLOAT, smoothCameraSpeed),
	MEMBER(MemberType::FLOAT, aimingCameraSpeed),
	MEMBER(MemberType::FLOAT, aimingDistance),
	MEMBER(MemberType::BOOL, useSmoothCamera),
	MEMBER_SEPARATOR("Shaker Control"),
	MEMBER(MemberType::FLOAT, shakeTotalTime),
	MEMBER(MemberType::FLOAT, shakeTimer),
	MEMBER(MemberType::FLOAT, shakeMultiplier),
	MEMBER(MemberType::BOOL, useAimingCamera),
};

GENERATE_BODY_IMPL(CameraController);

void CameraController::Start() {
	transform = GetOwner().GetComponent<ComponentTransform>();
	camera = GetOwner().GetComponent<ComponentCamera>();
	GameObject* playerControllerObj = GameplaySystems::GetGameObject(playerControllerObjUID);
	if (playerControllerObj) {
		playerController = GET_SCRIPT(playerControllerObj, PlayerController);
	}

	cameraInitialOffsetX = cameraOffsetX;
	cameraInitialOffsetY = cameraOffsetY;
	cameraInitialOffsetZ = cameraOffsetZ;

	RestoreCameraOffset();

	shakeMultiplierStoredValue = shakeMultiplier;
}

void CameraController::Update() {
	if (playerController == nullptr || transform == nullptr || camera == nullptr) return;
	if (playerController->IsPlayerDead()) return;
	if (GameplaySystems::GetGlobalVariable(globalCameraEventOn, false)) return;
	float3 playerGlobalPos = playerController->playerFang.playerMainTransform->GetGlobalPosition();
	float3 desiredPosition = playerGlobalPos + float3(cameraOffsetX, cameraOffsetY, cameraOffsetZ);
	float3 smoothedPosition = desiredPosition;

	if (useSmoothCamera) {
		if (useAimingCamera && (playerController->playerFang.IsAiming() || playerController->playerOnimaru.IsAiming())) {

			float2 mousePosition = float2(0, 0);

			if (GameplaySystems::GetGlobalVariable(globalUseGamepad, false) && Input::IsGamepadConnected(0)) {
				if (playerController->playerFang.playerMainTransform) {

					float2 orientationInput = playerController->playerFang.GetInputFloat2(InputActions::ORIENTATION);
					if (orientationInput.x != 0 || orientationInput.y != 0) {
						mousePosition = orientationInput;
						mousePosition = mousePosition.Mul(axisInverter).Normalized();
					}
				}
			} else {
				mousePosition = Input::GetMousePositionNormalized();
			}


			LineSegment ray = camera->frustum.UnProjectLineSegment(mousePosition.x, mousePosition.y);
			Plane p = Plane(playerGlobalPos, float3(0, 1, 0));
			float3 aimingPosition = playerGlobalPos + (p.ClosestPoint(ray) - playerGlobalPos) * aimingDistance + float3(cameraOffsetX, cameraOffsetY, cameraOffsetZ);

			smoothedPosition = float3::Lerp(transform->GetGlobalPosition(), aimingPosition, aimingCameraSpeed * Time::GetDeltaTime());

		} else {
			smoothedPosition = float3::Lerp(transform->GetGlobalPosition(), desiredPosition, smoothCameraSpeed * Time::GetDeltaTime());
		}
	}

	if (shakeTimer > 0 && !GameplaySystems::GetGlobalVariable(globalIsGameplayBlocked, true)) {
		float2 shakeDir = GetRandomPosInUnitaryCircle(float2(0, 0));
		transform->SetGlobalPosition(smoothedPosition + transform->GetRight() * shakeDir.x * shakeMultiplier + transform->GetUp() * shakeDir.y * shakeMultiplier);
		shakeTimer -= Time::GetDeltaTime();
		Screen::SetChromaticAberration(true);

	} else {
		transform->SetGlobalPosition(smoothedPosition);
		Screen::SetChromaticAberration(false);
		shakeMultiplier = shakeMultiplierStoredValue;
	}
}



void CameraController::StartShake(float shakeMult) {
	shakeTimer = shakeTotalTime;
	if (shakeMult > 0) shakeMultiplier = shakeMult;
}

void CameraController::ChangeCameraOffset(float x, float y, float z) {
	cameraOffsetZ = z;
	cameraOffsetY = y;
	cameraOffsetX = x;
}

void CameraController::RestoreCameraOffset() {
	cameraOffsetZ = cameraInitialOffsetZ;
	cameraOffsetY = cameraInitialOffsetY;
	cameraOffsetX = cameraInitialOffsetX;
}


float2 CameraController::GetRandomPosInUnitaryCircle(float2 center) {
	float random = (static_cast<float>(rand() % 101)) / 100.0f;
	float random2 = (static_cast<float>(rand() % 101)) / 100.0f;

	float r = sqrt(random);
	float theta = random2 * 2.0f * PI;
	float x = center.x + r * cos(theta);
	float y = center.y + r * sin(theta);

	return float2(x, y);
}