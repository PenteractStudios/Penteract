#pragma once

#include "Scripting/Script.h"
#include "Math/Quat.h"

class CameraController;
class PlayerController;
class ComponentTransform;
class CanvasFader;
class BossSceneCameraTravel;

class BossSceneCameraEvent : public Script {
	GENERATE_BODY(BossSceneCameraEvent);

public:

	void Start() override;
	void Update() override;

public:
	UID playerControllerObjUID = 0;
	UID canvasFaderObjUID = 0;


	float travellingAcceleration = 2.0f;
	float travellingDeceleration = 2.0f;
	float maxTravellingSpeed = 30.0f;
	float minTravellingSpeed = 10.0f;
	float decelerationDistance = 5.0f;
	float finishDistanceThreshold = 0.3f;
	float fadeInDuration = 0.3f;
	float fadeOutDuration = 0.3f;

private:

	void TravelToCurrentTargetPointUpdate();
	void SetPositionToCurrentTargetPoint();
	void NextCameraTravel();
	void BackToNormalGameplay();
	void UseTravelForValues(BossSceneCameraTravel* travel);
	void UseTravelDefaultValues();
private:

	enum class EventState {
		START,
		FADE_IN,
		TRAVELLING,
		FADE_OUT
	};


	float defaultTravellingAcceleration = 2.0f;
	float defaultTravellingDeceleration = 2.0f;
	float defaultMaxTravellingSpeed = 30.0f;
	float defaultMinTravellingSpeed = 10.0f;
	float defaultDecelerationDistance = 5.0f;
	float defaultFinishDistanceThreshold = 0.3f;

	EventState eventState = EventState::START;

	PlayerController* playerController = nullptr;
	CameraController* cameraController = nullptr;
	ComponentTransform* cameraTransform = nullptr;
	CanvasFader* canvasFader = nullptr;
	float3 originPosition;
	bool finished = false;
	int currentIndex = -1;
	int totalEvents = -1;
	float travellingSpeed = 0.0f;
	std::vector <ComponentTransform*>childrenTransforms;
	float3 currentTargetPosition = float3(0, 0, 0);
	Quat cameraOriginalRotation = Quat(0, 0, 0, 1);
};

