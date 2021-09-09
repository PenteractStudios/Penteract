#include "CameraController.h"

#include "PlayerController.h"
#include "Components/ComponentTransform.h"

#define PI 3.14159


EXPOSE_MEMBERS(CameraController) {
	MEMBER(MemberType::GAME_OBJECT_UID, playerControllerObjUID),
	MEMBER_SEPARATOR("Camera Positioning"),
	MEMBER(MemberType::FLOAT, cameraOffsetX),
	MEMBER(MemberType::FLOAT, cameraOffsetY),
	MEMBER(MemberType::FLOAT, cameraOffsetZ),
	MEMBER(MemberType::FLOAT, smoothCameraSpeed),
	MEMBER(MemberType::FLOAT, aimingDistance),
	MEMBER(MemberType::BOOL, useSmoothCamera),
	MEMBER_SEPARATOR("Shaker Control"),
	MEMBER(MemberType::FLOAT, shakeTotalTime),
	MEMBER(MemberType::FLOAT, shakeTimer),
	MEMBER(MemberType::FLOAT, shakeMultiplier)

};

GENERATE_BODY_IMPL(CameraController);

void CameraController::Start() {
	transform = GetOwner().GetComponent<ComponentTransform>();
	GameObject* playerControllerObj = GameplaySystems::GetGameObject(playerControllerObjUID);
	if (playerControllerObj) {
		playerController = GET_SCRIPT(playerControllerObj, PlayerController);
	}
	
	cameraInitialOffsetX = cameraOffsetX;
	cameraInitialOffsetY = cameraOffsetY;
	cameraInitialOffsetZ = cameraOffsetZ;

	RestoreCameraOffset();
}

void CameraController::Update() {

	if (playerController == nullptr || transform == nullptr) return;
	float3 playerGlobalPos = playerController->playerFang.playerMainTransform->GetGlobalPosition();

	float3 desiredPosition = playerGlobalPos + float3(cameraOffsetX, cameraOffsetY, cameraOffsetZ);
	//float3 aimingPositionPositiveX = playerGlobalPos + float3(cameraOffsetX + aimingDistance, cameraOffsetY, cameraOffsetZ);
	//float3 aimingPositionNegativeX = playerGlobalPos + float3(cameraOffsetX - aimingDistance, cameraOffsetY, cameraOffsetZ);

	//float3 aimingPositionPositiveY = playerGlobalPos + float3(cameraOffsetX , cameraOffsetY + aimingDistance, cameraOffsetZ);
	//float3 aimingPositionNegativeY = playerGlobalPos + float3(cameraOffsetX , cameraOffsetY - aimingDistance, cameraOffsetZ);

	float3 smoothedPosition = desiredPosition;
		
	if (useSmoothCamera) {
		if (playerController->playerFang.isAiming()) {
			float2 mousePosition = Input::GetMousePositionNormalized();

			if (mousePosition.x > 0.75) { aimingPositionX = aimingDistance; }
			else if (mousePosition.x < -0.75) { aimingPositionX = -aimingDistance; }
			else { aimingPositionX = 0.0f; }

			if (mousePosition.y > 0.75) { aimingPositionZ = -aimingDistance; }
			else if (mousePosition.y < -0.75) { aimingPositionZ = aimingDistance; }
			else { aimingPositionZ = 0.0f; }

			aimingPosition = playerGlobalPos + float3(cameraOffsetX + aimingPositionX, cameraOffsetY , cameraOffsetZ + aimingPositionZ);

			smoothedPosition = float3::Lerp(transform->GetGlobalPosition(), aimingPosition, smoothCameraSpeed * Time::GetDeltaTime());
			
		}
		else {
			smoothedPosition = float3::Lerp(transform->GetGlobalPosition(), desiredPosition, smoothCameraSpeed * Time::GetDeltaTime());
		}
	}

	if (shakeTimer > 0) {
		float2 shakeDir = GetRandomPosInUnitaryCircle(float2(0, 0));
		transform->SetGlobalPosition(smoothedPosition + transform->GetRight() * shakeDir.x * shakeMultiplier + transform->GetUp() * shakeDir.y * shakeMultiplier);
		shakeTimer -= Time::GetDeltaTime();
	} else {
		transform->SetGlobalPosition(smoothedPosition);
	}
}



void CameraController::StartShake() {
	shakeTimer = shakeTotalTime;
}

void CameraController::ChangeCameraOffset(float x, float y, float z)
{
	cameraOffsetZ = z;
	cameraOffsetY = y;
	cameraOffsetX = x;
}

void CameraController::RestoreCameraOffset()
{
	cameraOffsetZ = cameraInitialOffsetZ;
	cameraOffsetY = cameraInitialOffsetY;
	cameraOffsetX = cameraInitialOffsetX;
}


float2 CameraController::GetRandomPosInUnitaryCircle(float2 center) {
	float random = (static_cast<float>(rand() % 101)) / 100.0f;
	float random2 = (static_cast<float>(rand() % 101)) / 100.0f;

	float r = sqrt(random);
	float theta = random2 * 2 * PI;
	float x = center.x + r * cos(theta);
	float y = center.y + r * sin(theta);

	return float2(x, y);
}