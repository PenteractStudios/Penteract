#include "CheckpointManager.h"
#include "GameController.h"
#include "GameplaySystems.h"
#include "GameObject.h"
#include "PlayerController.h"
#include "Components/ComponentTransform.h"
#include "GlobalVariables.h"

EXPOSE_MEMBERS(CheckpointManager) {
	MEMBER(MemberType::GAME_OBJECT_UID, avatarUID),
	MEMBER(MemberType::FLOAT, distanceThreshold),
	MEMBER(MemberType::FLOAT, timeBetweenChecks),
	MEMBER_SEPARATOR("CHECKPOINT POSITIONS IN LEVEL"),
	MEMBER(MemberType::FLOAT3, checkpointPosition1),
	MEMBER(MemberType::FLOAT3, checkpointPosition2),
	MEMBER(MemberType::FLOAT3, checkpointPosition3),
	MEMBER(MemberType::FLOAT3, checkpointPosition4),
	MEMBER(MemberType::FLOAT3, checkpointPosition5),
	MEMBER_SEPARATOR("ENCOUNTERS LEVEL"),
	MEMBER(MemberType::GAME_OBJECT_UID, encounter1UID),
	MEMBER(MemberType::GAME_OBJECT_UID, encounter2UID),
	MEMBER(MemberType::GAME_OBJECT_UID, encounter3UID),
	MEMBER(MemberType::GAME_OBJECT_UID, encounter4UID),
	MEMBER(MemberType::GAME_OBJECT_UID, encounter5UID),
	MEMBER(MemberType::GAME_OBJECT_UID, encounter6UID),
	MEMBER(MemberType::GAME_OBJECT_UID, encounter7UID),
	MEMBER_SEPARATOR("DOORS & LASERS LEVEL"),
	MEMBER(MemberType::GAME_OBJECT_UID, doors1UID),
	MEMBER(MemberType::GAME_OBJECT_UID, doors2UID),
	MEMBER(MemberType::GAME_OBJECT_UID, doors3UID),
	MEMBER(MemberType::GAME_OBJECT_UID, doors4UID),
	MEMBER(MemberType::GAME_OBJECT_UID, doors5UID),
};

GENERATE_BODY_IMPL(CheckpointManager);

void CheckpointManager::Start() {

	/* Audio */
	selectable = GetOwner().GetComponent<ComponentSelectable>();

	int i = 0;
	for (ComponentAudioSource& src : GetOwner().GetComponents<ComponentAudioSource>()) {
		if (i < static_cast<int>(UIAudio::TOTAL)) audios[i] = &src;
		++i;
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

	// TODO: This was breaking the initial position of the player when coming from another scene
	// transform->SetGlobalPosition(runtimeCheckpointPositions[GameplaySystems::GetGlobalVariable(globalCheckpoint, 0)]);
	
	playerScript = GET_SCRIPT(avatarObj, PlayerController);

	// TODO: Assign UID to GameObject
	encounter1 = GameplaySystems::GetGameObject(encounter1UID);
	encounter2 = GameplaySystems::GetGameObject(encounter2UID);
	encounter3 = GameplaySystems::GetGameObject(encounter3UID);
	encounter4 = GameplaySystems::GetGameObject(encounter4UID);
	encounter5 = GameplaySystems::GetGameObject(encounter5UID);
	encounter6 = GameplaySystems::GetGameObject(encounter6UID);
	encounter7 = GameplaySystems::GetGameObject(encounter7UID);

	doors1 = GameplaySystems::GetGameObject(doors1UID);
	doors2 = GameplaySystems::GetGameObject(doors2UID);
	doors3 = GameplaySystems::GetGameObject(doors3UID);
	doors4 = GameplaySystems::GetGameObject(doors4UID);
	doors5 = GameplaySystems::GetGameObject(doors5UID);

	if (!encounter1 && !encounter2 && !encounter3 && !encounter4 && !encounter5 && !encounter6 && !encounter7) return;
	if (!doors1 && !doors2 && !doors3 && !doors4 && !doors5) return;

	/* Controller every particularity of the level */

	if (GameplaySystems::GetGlobalVariable(globalLevel, 0) == 1) {
		if (GameplaySystems::GetGlobalVariable(globalCheckpoint, 0) > 0) {
			/* Control encounters by actual checkpoint */
			switch (1) {
			case 1:
				encounter1->Disable();
				if (GameplaySystems::GetGlobalVariable(globalCheckpoint, 0) == 1) break;
			case 2:
				encounter2->Disable();
				doors1->Disable();
				doors2->Disable();
				if (GameplaySystems::GetGlobalVariable(globalCheckpoint, 0) == 2) break;
			case 3:
				if (GameplaySystems::GetGlobalVariable(globalCheckpoint, 0) == 3) break;
			case 4:
				if (GameplaySystems::GetGlobalVariable(globalCheckpoint, 0) == 4) break;
			default:
				break;
			}
		}
	}

	if (GameplaySystems::GetGlobalVariable(globalLevel, 0) == 2) {
		if (GameplaySystems::GetGlobalVariable(globalCheckpoint, 0) > 0) {
			/* Control encounters by actual checkpoint */
			switch (1) {
			case 1:
				if (GameplaySystems::GetGlobalVariable(globalCheckpoint, 0) == 1) break;
			case 2:

				if (GameplaySystems::GetGlobalVariable(globalCheckpoint, 0) == 2) break;
			case 3:

				if (GameplaySystems::GetGlobalVariable(globalCheckpoint, 0) == 3) break;
			default:
				break;
			}
		}
	}
}

void CheckpointManager::CheckDistanceWithCheckpoints() {
	if (!avatarObj) return;
	int checkPointCloseEnough = -1;
	ComponentTransform* transform = avatarObj->GetComponent<ComponentTransform>();
	for (int i = 0; i < N_CHECKPOINTS && checkPointCloseEnough == -1; i++) {
		if (runtimeCheckpointPositions[i].Distance(transform->GetGlobalPosition()) < distanceThreshold) {
			checkPointCloseEnough = i;
			GameplaySystems::SetGlobalVariable(globalCheckpoint, i);
		}
	}
}

void CheckpointManager::Update() {
	if (timeBetweenChecks < timeBetweenChecksCounter) {
		CheckDistanceWithCheckpoints();
		timeBetweenChecksCounter = 0;
	}

	if (dirty) {
		dirty = false;
		if (!avatarObj) return;
		ComponentTransform* transform = avatarObj->GetComponent<ComponentTransform>();
		if (!playerScript) return;
		agent = playerScript->playerFang.agent; // The first time to load the game (always be Fang)
		if (!agent && !transform) return;
		agent->RemoveAgentFromCrowd();
		transform->SetGlobalPosition(runtimeCheckpointPositions[GameplaySystems::GetGlobalVariable(globalCheckpoint, 0)]);
		agent->AddAgentToCrowd();
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
			if (!playerScript) return;
			if (playerScript->playerFang.characterGameObject->IsActive()) {
				agent = playerScript->playerFang.agent;
			} else {
				agent = playerScript->playerOnimaru.agent;
			}
			if (!agent && !transform) return;
			agent->RemoveAgentFromCrowd();
			GameplaySystems::SetGlobalVariable(globalCheckpoint, checkpointToSet);
			transform->SetGlobalPosition(runtimeCheckpointPositions[GameplaySystems::GetGlobalVariable(globalCheckpoint, 0)]);
			agent->AddAgentToCrowd();
		}
	}
	/////////////////////////////////////Debug function (GODMODE?)/////////////////////////////////////////
	/////////////////////////////////////Debug function (GODMODE?)/////////////////////////////////////////

	timeBetweenChecksCounter += Time::GetDeltaTime();

	/* Audio sounds */
	if (selectable) {
		ComponentEventSystem* eventSystem = UserInterface::GetCurrentEventSystem();
		if (eventSystem) {
			ComponentSelectable* hoveredComponent = eventSystem->GetCurrentlyHovered();
			if (hoveredComponent) {
				bool hovered = selectable->GetID() == hoveredComponent->GetID() ? true : false;
				if (hovered) {
					if (playHoveredAudio) {
						PlayAudio(UIAudio::HOVERED);
						playHoveredAudio = false;
					}
				}
				else {
					playHoveredAudio = true;
				}
			}
			else {
				playHoveredAudio = true;
			}
		}
	}
}

void CheckpointManager::OnButtonClick() {
	PlayAudio(UIAudio::CLICKED);
}

void CheckpointManager::PlayAudio(UIAudio type) {
	if (audios[static_cast<int>(type)]) audios[static_cast<int>(type)]->Play();
}

