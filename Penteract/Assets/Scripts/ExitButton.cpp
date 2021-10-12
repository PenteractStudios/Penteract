#include "ExitButton.h"

#include "SceneTransition.h"
#include "GameplaySystems.h"
#include "GameObject.h"

EXPOSE_MEMBERS(ExitButton) {
	// Add members here to expose them to the engine. Example:
	// MEMBER(MemberType::BOOL, exampleMember1),
	MEMBER(MemberType::GAME_OBJECT_UID, transitionUID)
};

GENERATE_BODY_IMPL(ExitButton);

void ExitButton::Start() {
	selectable = GetOwner().GetComponent<ComponentSelectable>();

	if (transitionUID != 0) {
		transitionGO = GameplaySystems::GetGameObject(transitionUID);
		if (transitionGO) sceneTransition = GET_SCRIPT(transitionGO, SceneTransition);
	}
}

void ExitButton::Update() {}

void ExitButton::OnButtonClick() {
    
	if (sceneTransition) {
		sceneTransition->StartTransition(true);
	} else {
		SceneManager::ExitGame();
	}
}