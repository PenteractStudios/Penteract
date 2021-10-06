#include "StartTitleGlitchOnPlay.h"
#include "GlitchyTitleController.h"

#include "GameplaySystems.h"
#include "CheckpointManager.h"
#include "SceneTransition.h"
#include "PlayerController.h"

#include "GameplaySystems.h"
#include "GameObject.h"
#include "VideoScene1Start.h"

#include "GlobalVariables.h" 


EXPOSE_MEMBERS(StartTitleGlitchOnPlay) {
	MEMBER(MemberType::SCENE_RESOURCE_UID, sceneUID),
	MEMBER(MemberType::GAME_OBJECT_UID, controllerObjUID),
	MEMBER(MemberType::GAME_OBJECT_UID, parentCanvasUID),
};

GENERATE_BODY_IMPL(StartTitleGlitchOnPlay);

void StartTitleGlitchOnPlay::Start() {
	GameObject* controllerObj = GameplaySystems::GetGameObject(controllerObjUID);
	if (controllerObj) {
		controller = GET_SCRIPT(controllerObj, GlitchyTitleController);
	}

	parentCanvas = GameplaySystems::GetGameObject(parentCanvasUID);

	/* Audio */
	selectable = GetOwner().GetComponent<ComponentSelectable>();

	int i = 0;
	for (ComponentAudioSource& src : GetOwner().GetComponents<ComponentAudioSource>()) {
		if (i < static_cast<int>(UIAudio::TOTAL)) audios[i] = &src;
		++i;
	}
}

void StartTitleGlitchOnPlay::Update() {
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
				} else {
					playHoveredAudio = true;
				}
			} else {
				playHoveredAudio = true;
			}
		}
	}
}

void StartTitleGlitchOnPlay::OnButtonClick() {

	PlayAudio(UIAudio::CLICKED);

	if (controller) {
		controller->PressedPlay(this);
	} else {
		DoTransition();
	} 
	
	if (parentCanvas) parentCanvas->Disable();	// Deactivates the whole menu when Start is clicked

}

void StartTitleGlitchOnPlay::DoTransition() {
	if (sceneUID != 0) {
		GameplaySystems::SetGlobalVariable(globalVariableKeyPlayVideoScene1, true);

		SceneManager::ChangeScene(sceneUID);

		PlayerController::currentLevel = 1;
		Player::level1Upgrade = false;
		Player::level2Upgrade = false;

		if (Time::GetDeltaTime() == 0.f) Time::ResumeGame();
	}
}


void StartTitleGlitchOnPlay::PlayAudio(UIAudio type) {
	if (audios[static_cast<int>(type)]) audios[static_cast<int>(type)]->Play();
}