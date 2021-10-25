#include "CheckpointButtonsController.h"

#include "GameplaySystems.h"
#include "GlobalVariables.h" 

#include <string>

EXPOSE_MEMBERS(CheckpointButtonsController) {
	MEMBER_SEPARATOR("Buttons Level 1"),
	MEMBER(MemberType::GAME_OBJECT_UID, btnLevel1_Checkpoint0UID),
	MEMBER(MemberType::GAME_OBJECT_UID, btnLevel1_Checkpoint1UID),
	MEMBER(MemberType::GAME_OBJECT_UID, btnLevel1_Checkpoint2UID),
	MEMBER(MemberType::GAME_OBJECT_UID, btnLevel1_Checkpoint3UID),
	MEMBER(MemberType::GAME_OBJECT_UID, btnLevel1_Checkpoint4UID),
	MEMBER_SEPARATOR("Buttons Level 2"),
	MEMBER(MemberType::GAME_OBJECT_UID, btnLevel2_Checkpoint0UID),
	MEMBER(MemberType::GAME_OBJECT_UID, btnLevel2_Checkpoint1UID),
	MEMBER(MemberType::GAME_OBJECT_UID, btnLevel2_Checkpoint2UID),
	MEMBER(MemberType::GAME_OBJECT_UID, btnLevel2_Checkpoint3UID),
	MEMBER_SEPARATOR("Buttons Level 3"),
	MEMBER(MemberType::GAME_OBJECT_UID, btnLevel3_Checkpoint0UID),
};

GENERATE_BODY_IMPL(CheckpointButtonsController);

void CheckpointButtonsController::Start() {

	btnLevel1_Checkpoint0 = GameplaySystems::GetGameObject(btnLevel1_Checkpoint0UID);
	btnLevel1_Checkpoint1 = GameplaySystems::GetGameObject(btnLevel1_Checkpoint1UID);
	btnLevel1_Checkpoint2 = GameplaySystems::GetGameObject(btnLevel1_Checkpoint2UID);
	btnLevel1_Checkpoint3 = GameplaySystems::GetGameObject(btnLevel1_Checkpoint3UID);
	btnLevel1_Checkpoint4 = GameplaySystems::GetGameObject(btnLevel1_Checkpoint4UID);
	btnLevel2_Checkpoint0 = GameplaySystems::GetGameObject(btnLevel2_Checkpoint0UID);
	btnLevel2_Checkpoint1 = GameplaySystems::GetGameObject(btnLevel2_Checkpoint1UID);
	btnLevel2_Checkpoint2 = GameplaySystems::GetGameObject(btnLevel2_Checkpoint2UID);
	btnLevel2_Checkpoint3 = GameplaySystems::GetGameObject(btnLevel2_Checkpoint3UID);
	btnLevel3_Checkpoint0 = GameplaySystems::GetGameObject(btnLevel3_Checkpoint0UID);

	if (!btnLevel1_Checkpoint0 && !btnLevel1_Checkpoint1 && !btnLevel1_Checkpoint2 && !btnLevel1_Checkpoint3 && !btnLevel1_Checkpoint4
		&& !btnLevel2_Checkpoint0 && !btnLevel2_Checkpoint1 && !btnLevel2_Checkpoint2 && !btnLevel2_Checkpoint3 && !btnLevel3_Checkpoint0) return;

	buttonsLevel1.push_back(btnLevel1_Checkpoint0);
	buttonsLevel1.push_back(btnLevel1_Checkpoint1);
	buttonsLevel1.push_back(btnLevel1_Checkpoint2);
	buttonsLevel1.push_back(btnLevel1_Checkpoint3);
	buttonsLevel1.push_back(btnLevel1_Checkpoint4);

	buttonsLevel2.push_back(btnLevel2_Checkpoint0);
	buttonsLevel2.push_back(btnLevel2_Checkpoint1);
	buttonsLevel2.push_back(btnLevel2_Checkpoint2);
	buttonsLevel2.push_back(btnLevel2_Checkpoint3);

	/* TESTING */
	GameplaySystems::SetGlobalVariable(globalLevel, 2);
	GameplaySystems::SetGlobalVariable(globalCheckpoint, 3);

	actualLevel = GameplaySystems::GetGlobalVariable(globalLevel, 1);
	actualCheckpoint = GameplaySystems::GetGlobalVariable(globalCheckpoint, 0);

	switch (actualLevel) {
		case 1:
			for (int cp = 0; cp < buttonsLevel1.size(); cp++) {
				if (cp > actualCheckpoint) buttonsLevel1[cp]->Disable();
			}
			for (GameObject* button : buttonsLevel2) {
				button->Disable();
			}
			btnLevel3_Checkpoint0->Disable();
			break;
		case 2:
			for (int cp = 0; cp < buttonsLevel2.size(); cp++) {
				if (cp > actualCheckpoint) buttonsLevel2[cp]->Disable();
			}
			btnLevel3_Checkpoint0->Disable();
			break;
		case 3:
			break;
		default:
			break;
	}
}

void CheckpointButtonsController::Update() {
	
}