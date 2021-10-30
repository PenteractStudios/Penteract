#include "CheckpointManager.h"
#include "GameController.h"
#include "GameplaySystems.h"
#include "GameObject.h"
#include "PlayerController.h"
#include "Components/ComponentTransform.h"
#include "GlobalVariables.h"
#include "UpdateCheckpointVariable.h"

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
	MEMBER_SEPARATOR("TRIGGERS GENERAL GAMEOBJECT"),
	MEMBER(MemberType::GAME_OBJECT_UID, triggersUID),
	MEMBER_SEPARATOR("ENCOUNTERS LEVEL"),
	MEMBER(MemberType::GAME_OBJECT_UID, encounter1UID),
	MEMBER(MemberType::GAME_OBJECT_UID, encounter2UID),
	MEMBER(MemberType::GAME_OBJECT_UID, encounter3UID),
	MEMBER(MemberType::GAME_OBJECT_UID, encounter4UID),
	MEMBER(MemberType::GAME_OBJECT_UID, encounter5UID),
	MEMBER(MemberType::GAME_OBJECT_UID, encounter6UID),
	MEMBER_SEPARATOR("DOORS & LASERS LEVEL"),
	MEMBER(MemberType::GAME_OBJECT_UID, doors1UID),
	MEMBER(MemberType::GAME_OBJECT_UID, doors2UID),
	MEMBER(MemberType::GAME_OBJECT_UID, doors3UID),
	MEMBER(MemberType::GAME_OBJECT_UID, doors4UID),
	MEMBER_SEPARATOR("DIALOGS LEVEL"),
	MEMBER(MemberType::GAME_OBJECT_UID, dialogs1UID),
	MEMBER(MemberType::GAME_OBJECT_UID, dialogs2UID),
	MEMBER(MemberType::GAME_OBJECT_UID, dialogs3UID),
	MEMBER(MemberType::GAME_OBJECT_UID, dialogs4UID),  
	MEMBER_SEPARATOR("UPGRADES LEVEL"),
	MEMBER(MemberType::GAME_OBJECT_UID, upgrades1UID),
	MEMBER(MemberType::GAME_OBJECT_UID, upgrades2UID),
	MEMBER(MemberType::GAME_OBJECT_UID, upgrades3UID),
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
	
	playerScript = GET_SCRIPT(avatarObj, PlayerController);

	triggers = GameplaySystems::GetGameObject(triggersUID);

	encounter1 = GameplaySystems::GetGameObject(encounter1UID);
	encounter2 = GameplaySystems::GetGameObject(encounter2UID);
	encounter3 = GameplaySystems::GetGameObject(encounter3UID);
	encounter4 = GameplaySystems::GetGameObject(encounter4UID);
	encounter5 = GameplaySystems::GetGameObject(encounter5UID);
	encounter6 = GameplaySystems::GetGameObject(encounter6UID);

	doors1 = GameplaySystems::GetGameObject(doors1UID);
	doors2 = GameplaySystems::GetGameObject(doors2UID);
	doors3 = GameplaySystems::GetGameObject(doors3UID);
	doors4 = GameplaySystems::GetGameObject(doors4UID);

	dialogs1 = GameplaySystems::GetGameObject(dialogs1UID);
	dialogs2 = GameplaySystems::GetGameObject(dialogs2UID);
	dialogs3 = GameplaySystems::GetGameObject(dialogs3UID);
	dialogs4 = GameplaySystems::GetGameObject(dialogs4UID);

	upgrades1 = GameplaySystems::GetGameObject(upgrades1UID);
	upgrades2 = GameplaySystems::GetGameObject(upgrades2UID);
	upgrades3 = GameplaySystems::GetGameObject(upgrades3UID);

	/* Disabled the upgrade that are already enabled */
	if (GameplaySystems::GetGlobalVariable(globalLevel, 0) == 1) {
		if (GameplaySystems::GetGlobalVariable(globalUpgradeLevel1_Plaza, false)) {
			playerScript->obtainedUpgradeCells += 1;
			if (upgrades1) upgrades1->Disable();
		}
		if (GameplaySystems::GetGlobalVariable(globalUpgradeLevel1_Cafeteria, false)) {
			playerScript->obtainedUpgradeCells += 1;
			if (upgrades2) upgrades2->Disable();
		}
		if (GameplaySystems::GetGlobalVariable(globalUpgradeLevel1_Presecurity, false)) {
			playerScript->obtainedUpgradeCells += 1;
			if (upgrades3) upgrades3->Disable();
		}
	}

	if (GameplaySystems::GetGlobalVariable(globalLevel, 0) == 2) {
		if (GameplaySystems::GetGlobalVariable(globalUpgradeLevel2_Catwalks, false)) {
			playerScript->obtainedUpgradeCells += 1;
			if (upgrades1) upgrades1->Disable();
		}
		if (GameplaySystems::GetGlobalVariable(globalUpgradeLevel2_AfterArena1, false)) {
			playerScript->obtainedUpgradeCells += 1;
			if (upgrades2) upgrades2->Disable();
		}
		if (GameplaySystems::GetGlobalVariable(globalUpgradeLevel2_FireBridge, false)) {
			playerScript->obtainedUpgradeCells += 1;
			if (upgrades3) upgrades3->Disable();
		}
	}

	/* Disabled the triggers of the checkpoint that already passed - CheckpointTriggers */
	if (triggers) {
		listTriggers = triggers->GetChildren();
		for (int i = 0; i < listTriggers.size(); i++) {
			if (i < GameplaySystems::GetGlobalVariable(globalCheckpoint, 0)) {
				// Disabled the trigger
				ComponentBoxCollider* boxCollider = listTriggers[i]->GetComponent<ComponentBoxCollider>();
				if (boxCollider) boxCollider->Disable();
				listTriggers[i]->Disable();
			}
		}
	}

	/* Controller every particularity of the level */
	if (GameplaySystems::GetGlobalVariable(globalLevel, 0) == 1) {
		if (GameplaySystems::GetGlobalVariable(globalCheckpoint, 0) > 0) {
			/* Control encounters by actual checkpoint */
			switch (1) {
			case 1: // After Plaza - In Cafeteria
				GameplaySystems::SetGlobalVariable(globalVariableKeyPlayVideoScene1, false);
				GameplaySystems::SetGlobalVariable(isVideoActive, false);
				GameplaySystems::SetGlobalVariable(globalSkill1TutorialReached, true);
				GameplaySystems::SetGlobalVariable(globalSkill2TutorialReached, true);
				GameplaySystems::SetGlobalVariable(globalSkill3TutorialReached, true);
				if (dialogs1) dialogs1->Disable();											// Trigger Fang Tutorial 1
				if (encounter1) encounter1->Disable();										// Encounter_Plaza
				if (doors4) doors4->Disable();												// Its Duke object! - Duke1
				if (GameplaySystems::GetGlobalVariable(globalCheckpoint, 0) == 1) break;
			case 2: // After Cafeteria - In Transport
				GameplaySystems::SetGlobalVariable(globalSwitchTutorialReached, true);
				GameplaySystems::SetGlobalVariable(globalSkill1TutorialReachedOni, true);
				GameplaySystems::SetGlobalVariable(globalSkill2TutorialReachedOni, true);
				GameplaySystems::SetGlobalVariable(globalSkill3TutorialReachedOni, true);
				if (dialogs2) dialogs2->Disable();											// TriggerSwapCharacters
				if (encounter2) encounter2->Disable();										// Encounter_Cafeteria
				if (doors1) doors1->Disable();												// DoorObstacle - Plaza
				if (doors2) doors2->Disable();												// DoorObstacle - Cafeteria
				if (GameplaySystems::GetGlobalVariable(globalCheckpoint, 0) == 2) break;
			case 3: // After Transport - In Pre-security
				if (encounter3) encounter3->Disable();										// Encounter Transport
				if (encounter4) encounter4->Disable();										// Encounter Patrolling
				if (GameplaySystems::GetGlobalVariable(globalCheckpoint, 0) == 3) break;
			case 4: // After Pre-security - In Security
				if (encounter5) encounter5->Disable();										// Encounter Presecurity
				if (encounter6) encounter6->Disable();										// Encounter Catwalk
				if (doors3) doors3->Disable();												// Lasers Presecurity
				if (dialogs3) dialogs3->Disable();											// Scene - WalkToFactory
				if (dialogs4) dialogs4->Disable();											// Its Duke Object - Duke2
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
			case 1: // After Elevator - In Test Arena 1
				if (encounter1) encounter1->Disable();										// Encounter Catwalks
				if (doors1) doors1->Disable();												// Door Obstacle Elevator
				if (doors2) doors2->Disable();												// Door Obstacle Catwalks Left
				if (dialogs1) dialogs1->Disable();											// Trigger StartScene
				if (GameplaySystems::GetGlobalVariable(globalCheckpoint, 0) == 1) break;
			case 2: // After Test Arena 1 - In Fire Bridge
				if (encounter2) encounter2->Disable();										// Encounter Arena 1
				if (encounter3) encounter3->Disable();										// Lasers Arena 1
				if (doors3) doors3->Disable();												// Door Obstacle Arena 1
				if (dialogs2) dialogs2->Disable();											// TriggerDukeBegin
				if (dialogs3) dialogs3->Disable();											// Its Duke Object - Duke
				if (dialogs4) dialogs4->Disable();											// TriggerDukeEnd
				if (GameplaySystems::GetGlobalVariable(globalCheckpoint, 0) == 2) break;
			case 3: // After Fire Bridge -  In Test Arena 2
				if (encounter4) encounter4->Enable();										// Enable triegger fire & door
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
		GameplaySystems::SetGlobalVariable(globalIsGameplayBlocked, false);
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

