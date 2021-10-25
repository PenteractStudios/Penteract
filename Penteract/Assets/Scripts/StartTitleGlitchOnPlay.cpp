#include "StartTitleGlitchOnPlay.h"
#include "GlitchyTitleController.h"

#include "GameplaySystems.h"
#include "PlayerController.h"
#include "GameObject.h"
#include "VideoScene1Start.h"

#include "GlobalVariables.h" 


EXPOSE_MEMBERS(StartTitleGlitchOnPlay) {
	MEMBER(MemberType::SCENE_RESOURCE_UID, sceneUID),
	MEMBER(MemberType::GAME_OBJECT_UID, controllerObjUID),
	MEMBER(MemberType::GAME_OBJECT_UID, parentCanvasUID),
	MEMBER_SEPARATOR("Control level & chekpoint"),
	MEMBER(MemberType::INT, levelSelected),
	MEMBER(MemberType::INT, checkpointSelected)
};

GENERATE_BODY_IMPL(StartTitleGlitchOnPlay);

void StartTitleGlitchOnPlay::Start() {
	/* Glitch controller */
	GameObject* controllerObj = GameplaySystems::GetGameObject(controllerObjUID);
	if (controllerObj) {
		controller = GET_SCRIPT(controllerObj, GlitchyTitleController);
	}

	parentCanvas = GameplaySystems::GetGameObject(parentCanvasUID);

	selectable = GetOwner().GetComponent<ComponentSelectable>();
}

void StartTitleGlitchOnPlay::Update() {}

void StartTitleGlitchOnPlay::OnButtonClick() {
	if (controller) {
		controller->PressedPlay(this);
	} else {
		DoTransition();
	} 
	
	if (parentCanvas) parentCanvas->Disable();	// Deactivates the whole menu when Start is clicked

}

void StartTitleGlitchOnPlay::DoTransition() {
	if (sceneUID != 0) {
		GameplaySystems::SetGlobalVariable(globalCheckpoint, checkpointSelected);
		GameplaySystems::SetGlobalVariable(globalLevel, levelSelected);

		GameplaySystems::SetGlobalVariable(globalVariableKeyPlayVideoScene1, true);

		if (GameplaySystems::GetGlobalVariable(globalLevel, 1) == 1 && GameplaySystems::GetGlobalVariable(globalCheckpoint, 0) == 0) {
			GameplaySystems::SetGlobalVariable(globalUpgradeLevel1_Plaza, false);
			GameplaySystems::SetGlobalVariable(globalUpgradeLevel1_Cafeteria, false);
			GameplaySystems::SetGlobalVariable(globalUpgradeLevel1_Presecurity, false);
		}

		if (GameplaySystems::GetGlobalVariable(globalLevel, 2) == 2 && GameplaySystems::GetGlobalVariable(globalCheckpoint, 0) == 0) {
			GameplaySystems::SetGlobalVariable(globalUpgradeLevel2_Catwalks, false);
			GameplaySystems::SetGlobalVariable(globalUpgradeLevel2_AfterArena1, false);
			GameplaySystems::SetGlobalVariable(globalUpgradeLevel2_FireBridge, false);
		}

		SceneManager::ChangeScene(sceneUID);

		PlayerController::currentLevel = GameplaySystems::GetGlobalVariable(globalLevel, 1);

		/* TODO: Control the upgrades*/
		Player::level1Upgrade = GameplaySystems::GetGlobalVariable(globalUpgradeLevel1_Plaza, false) && 
								GameplaySystems::GetGlobalVariable(globalUpgradeLevel1_Cafeteria, false) && 
								GameplaySystems::GetGlobalVariable(globalUpgradeLevel1_Presecurity, false);
		
		Player::level2Upgrade = GameplaySystems::GetGlobalVariable(globalUpgradeLevel2_Catwalks, false) &&
								GameplaySystems::GetGlobalVariable(globalUpgradeLevel2_AfterArena1, false) && 
								GameplaySystems::GetGlobalVariable(globalUpgradeLevel2_FireBridge, false);

		if (Time::GetDeltaTime() == 0.f) Time::ResumeGame();
	}
}