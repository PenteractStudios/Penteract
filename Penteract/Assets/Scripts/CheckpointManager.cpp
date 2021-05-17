#include "CheckpointManager.h"
#include "GameController.h"
#include "GameplaySystems.h"
#include "GameObject.h"
#include "Components/ComponentTransform.h"

EXPOSE_MEMBERS(CheckpointManager) {

	MEMBER(MemberType::GAME_OBJECT_UID, avatarUID),
		MEMBER(MemberType::FLOAT, distanceThreshold),
		MEMBER(MemberType::FLOAT, timeBetweenChecks),
		MEMBER(MemberType::FLOAT3, checkpointPosition1),
		MEMBER(MemberType::FLOAT3, checkpointPosition2),
		MEMBER(MemberType::FLOAT3, checkpointPosition3),
		MEMBER(MemberType::FLOAT3, checkpointPosition4),
		MEMBER(MemberType::FLOAT3, checkpointPosition5)
};

GENERATE_BODY_IMPL(CheckpointManager);

void CheckpointManager::Start() {
	//Error prevention
	if (checkpoint < 0 || checkpoint > N_CHECKPOINTS) {
		checkpoint = 0;
	}
	avatarObj = GameplaySystems::GetGameObject(avatarUID);

	if (timeBetweenChecks <= 0) {
		timeBetweenChecks = 0.3f;
	}

	runtimeCheckpointPositions[0] = checkpointPosition1;
	runtimeCheckpointPositions[1] = checkpointPosition2;
	runtimeCheckpointPositions[2] = checkpointPosition3;
	runtimeCheckpointPositions[3] = checkpointPosition4;
	runtimeCheckpointPositions[4] = checkpointPosition5;

	if (!avatarObj) return;
	ComponentTransform* transform = avatarObj->GetComponent<ComponentTransform>();

	if (!transform) return;

	transform->SetGlobalPosition(runtimeCheckpointPositions[checkpoint]);
}

void CheckpointManager::CheckDistanceWithCheckpoints() {
	if (!avatarObj)return;
	int checkPointCloseEnough = -1;
	ComponentTransform* transform = avatarObj->GetComponent<ComponentTransform>();
	for (int i = 0; i < N_CHECKPOINTS && checkPointCloseEnough == -1; i++) {
		if (runtimeCheckpointPositions[i].Distance(transform->GetGlobalPosition()) < distanceThreshold) {
			checkPointCloseEnough = i;
			checkpoint = i;
			std::string message = "New point is " + std::to_string(i);
			Debug::Log(message.c_str());

		}
	}

}

void CheckpointManager::Update() {
	if (timeBetweenChecks < timeBetweenChecksCounter) {
		CheckDistanceWithCheckpoints();
		timeBetweenChecksCounter = 0;
	}

	/////////////////////////////////////Debug function (GODMODE?)/////////////////////////////////////////
	/////////////////////////////////////Debug function (GODMODE?)/////////////////////////////////////////
	if (Debug::IsGodModeOn()) {
		int checkpointToSet = -1;
		if (Input::GetKeyCodeUp(Input::KEY_Y)) {
			checkpointToSet = 0;
		} else if (Input::GetKeyCodeUp(Input::KEY_U)) {
			checkpointToSet = 1;
		} else if (Input::GetKeyCodeUp(Input::KEY_I)) {
			checkpointToSet = 2;
		} else 	if (Input::GetKeyCodeUp(Input::KEY_O)) {
			checkpointToSet = 3;
		} else 	if (Input::GetKeyCodeUp(Input::KEY_P)) {
			checkpointToSet = 4;
		}

		if (checkpointToSet > -1) {
			if (!avatarObj) return;
			ComponentTransform* transform = avatarObj->GetComponent<ComponentTransform>();
			if (!transform) return;
			checkpoint = checkpointToSet;
			transform->SetGlobalPosition(runtimeCheckpointPositions[checkpointToSet]);
		}
	}
	/////////////////////////////////////Debug function (GODMODE?)/////////////////////////////////////////
	/////////////////////////////////////Debug function (GODMODE?)/////////////////////////////////////////

	timeBetweenChecksCounter += Time::GetDeltaTime();
}

