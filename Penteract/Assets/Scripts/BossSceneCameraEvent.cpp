#include "BossSceneCameraEvent.h"

#include "GameplaySystems.h"
#include "CameraController.h"
#include "PlayerController.h"
#include "CanvasFader.h"
#include "GlobalVariables.h"
#include "BossSceneCameraTravel.h"

EXPOSE_MEMBERS(BossSceneCameraEvent) {
	MEMBER(MemberType::GAME_OBJECT_UID, playerControllerObjUID),
	MEMBER(MemberType::GAME_OBJECT_UID, canvasFaderObjUID),
	MEMBER(MemberType::FLOAT, maxTravellingSpeed),
	MEMBER(MemberType::FLOAT, minTravellingSpeed),
	MEMBER(MemberType::FLOAT, travellingAcceleration),
	MEMBER(MemberType::FLOAT, travellingDeceleration),
	MEMBER(MemberType::FLOAT, decelerationDistance),
	MEMBER(MemberType::FLOAT, finishDistanceThreshold),
	MEMBER(MemberType::GAME_OBJECT_UID, skipTextObjUID),
	MEMBER(MemberType::FLOAT, fadeInDuration),
	MEMBER(MemberType::FLOAT, fadeOutDuration)
};

GENERATE_BODY_IMPL(BossSceneCameraEvent);

void BossSceneCameraEvent::Start() {
	travellingSpeed = minTravellingSpeed;
	GameObject* playerControllerObj = GameplaySystems::GetGameObject(playerControllerObjUID);
	if (playerControllerObj) {
		playerController = GET_SCRIPT(playerControllerObj, PlayerController);
		if (playerController) {
			if (playerController->camera) {
				cameraController = GET_SCRIPT(playerController->camera, CameraController);
				cameraTransform = cameraController->GetOwner().GetComponent<ComponentTransform>();

			}
		}
	}
	GameplaySystems::SetGlobalVariable(globalCameraEventOn, true);
	std::vector <GameObject* >children = GetOwner().GetChildren();

	for (GameObject* g : children) {
		if (g->IsActive())
			childrenTransforms.push_back(g->GetComponent<ComponentTransform>());
	}

	GameObject* canvasFaderObj = GameplaySystems::GetGameObject(canvasFaderObjUID);
	if (canvasFaderObj) {
		canvasFader = GET_SCRIPT(canvasFaderObj, CanvasFader);
	}

	skipTextObj = GameplaySystems::GetGameObject(skipTextObjUID);

	if (skipTextObj)skipTextObj->Enable(); //Enable at runtime if found, to prevent it from sticking around during gameplay if not assigned

	if (cameraTransform) {
		cameraOriginalRotation = cameraTransform->GetGlobalRotation();
	}

	totalEvents = children.size();

	defaultTravellingAcceleration = travellingAcceleration;
	defaultTravellingDeceleration = travellingDeceleration;
	defaultMaxTravellingSpeed = maxTravellingSpeed;
	defaultMinTravellingSpeed = minTravellingSpeed;
	defaultDecelerationDistance = decelerationDistance;
	defaultFinishDistanceThreshold = finishDistanceThreshold;


}

void BossSceneCameraEvent::Update() {
	if (!playerController || !cameraController || !cameraTransform || finished || !canvasFader) return;

	switch (eventState) {
	case EventState::START:
		eventState = EventState::TRAVELLING;
		NextCameraTravel();
		break;
	case EventState::FADE_IN:
		if (!canvasFader->IsPlaying()) {
			canvasFader->FadeIn(0.3f);
			eventState = EventState::TRAVELLING;
			travellingSpeed = minTravellingSpeed;
		}
		break;
	case EventState::FADE_OUT:
		if (!canvasFader->IsPlaying()) {
			NextCameraTravel();
			eventState = EventState::FADE_IN;
			break;
		}
		TravelToCurrentTargetPointUpdate();
		break;
	case EventState::TRAVELLING:
		if (!canvasFader->IsPlaying()) {
			if ((Input::GetKeyCodeDown(Input::KEYCODE::KEY_ESCAPE) || Input::GetControllerButtonDown(Input::SDL_CONTROLLER_BUTTON_START, 0))) {
				//SKIPPING
				canvasFader->FadeOut(0.1f);
				eventState = EventState::FADE_OUT;
				break;
			}
		}
		TravelToCurrentTargetPointUpdate();
		break;
	}

}

void BossSceneCameraEvent::TravelToCurrentTargetPointUpdate() {

	if (!cameraTransform) return;

	float3 cameraPos = cameraTransform->GetGlobalPosition();
	float3 vectorToTarget = childrenTransforms[currentIndex * 2 + 1]->GetGlobalPosition() - cameraPos;

	if (childrenTransforms[currentIndex * 2]->GetGlobalRotation().x == 0 && childrenTransforms[currentIndex * 2]->GetGlobalRotation().y == 0 && childrenTransforms[currentIndex * 2]->GetGlobalRotation().z == 0) {
		cameraTransform->SetGlobalRotation(cameraOriginalRotation);
	} else {
		cameraTransform->SetGlobalRotation(childrenTransforms[currentIndex * 2]->GetGlobalRotation());
	}

	cameraTransform->SetGlobalPosition(cameraPos + vectorToTarget.Normalized() * travellingSpeed * Time::GetDeltaTime());

	float distToTarget = vectorToTarget.Length();

	if (vectorToTarget.LengthSq() > decelerationDistance * decelerationDistance) {
		//Accelerating until max speed
		travellingSpeed = Min(travellingSpeed + travellingAcceleration * Time::GetDeltaTime(), maxTravellingSpeed);
	} else {
		eventState = EventState::FADE_OUT;
		if (!canvasFader->IsPlaying()) {
			canvasFader->FadeOut(0.3f);
		}
		//Decelerating
		travellingSpeed = Max(travellingSpeed - travellingDeceleration * Time::GetDeltaTime(), minTravellingSpeed);
	}
}

void BossSceneCameraEvent::SetPositionToCurrentTargetPoint() {
	cameraTransform->SetGlobalPosition(childrenTransforms[currentIndex * 2]->GetGlobalPosition());

	BossSceneCameraTravel* currentTravel = GET_SCRIPT(GetOwner().GetChild(currentIndex * 2), BossSceneCameraTravel);
	if (currentTravel) {
		UseTravelForValues(currentTravel);
	} else {
		UseTravelDefaultValues();
	}
}

void BossSceneCameraEvent::NextCameraTravel() {
	currentIndex++;

	if (currentIndex * 2 >= childrenTransforms.size()) {
		BackToNormalGameplay();
	} else {
		SetPositionToCurrentTargetPoint();
	}

}

void BossSceneCameraEvent::BackToNormalGameplay() {
	if (skipTextObj)skipTextObj->Disable();
	GameplaySystems::SetGlobalVariable(globalCameraEventOn, false);
	GetOwner().Disable();
	if (canvasFader) canvasFader->FadeIn(0.3f);
	if (cameraTransform) cameraTransform->SetGlobalRotation(cameraOriginalRotation);
}

void BossSceneCameraEvent::UseTravelForValues(BossSceneCameraTravel* travel) {
	travellingAcceleration = travel->travellingAcceleration;
	travellingDeceleration = travel->travellingDeceleration;
	maxTravellingSpeed = travel->maxTravellingSpeed;
	minTravellingSpeed = travel->startingTravellingSpeed;
	decelerationDistance = travel->decelerationDistance;
	finishDistanceThreshold = travel->finishDistanceThreshold;
}

void BossSceneCameraEvent::UseTravelDefaultValues() {
	travellingAcceleration = defaultTravellingAcceleration;
	travellingDeceleration = defaultTravellingDeceleration;
	maxTravellingSpeed = defaultMaxTravellingSpeed;
	minTravellingSpeed = defaultMinTravellingSpeed;
	decelerationDistance = defaultDecelerationDistance;
	finishDistanceThreshold = defaultFinishDistanceThreshold;
}
