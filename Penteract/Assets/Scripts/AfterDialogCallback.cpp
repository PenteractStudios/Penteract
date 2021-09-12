#include "AfterDialogCallback.h"

#include "FactoryDoors.h"
#include "GameplaySystems.h"
#include "GameObject.h"
#include "DialogueManager.h"

EXPOSE_MEMBERS(AfterDialogCallback) {
	MEMBER(MemberType::GAME_OBJECT_UID, gameControllerUID)
};

GENERATE_BODY_IMPL(AfterDialogCallback);

void AfterDialogCallback::Start() {
	GameObject* gameController = GameplaySystems::GetGameObject(gameControllerUID);
	if (gameController) dialogueManagerScript = GET_SCRIPT(gameController, DialogueManager);
	factoryDoorsScript = GET_SCRIPT(&GetOwner(), FactoryDoors);
}

void AfterDialogCallback::Update() {
	if (factoryDoorsScript && hasOpenedDialog && !dialogueManagerScript->HasActiveDialogue()) {
		factoryDoorsScript->Open();
		hasOpenedDialog = false;
	}
}

void AfterDialogCallback::OpenFactoryDoors() {
	hasOpenedDialog = true;
}