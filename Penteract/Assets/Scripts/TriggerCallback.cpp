#include "TriggerCallback.h"

#include "FactoryDoors.h"
#include "GameplaySystems.h"
#include "GameObject.h"
#include "DialogueManager.h"

EXPOSE_MEMBERS(TriggerCallback) {
	MEMBER(MemberType::GAME_OBJECT_UID, gameControllerUID)
};

GENERATE_BODY_IMPL(TriggerCallback);

void TriggerCallback::Start() {
	GameObject* gameController = GameplaySystems::GetGameObject(gameControllerUID);
	if (gameController) dialogueManagerScript = GET_SCRIPT(gameController, DialogueManager);
	factoryDoorsScript = GET_SCRIPT(&GetOwner(), FactoryDoors);
}

void TriggerCallback::Update() {
	if (factoryDoorsScript && hasOpenedDialog && !dialogueManagerScript->HasActiveDialogue()) {
		factoryDoorsScript->Open();
		hasOpenedDialog = false;
	}
}

void TriggerCallback::OpenFactoryDoors() {
	hasOpenedDialog = true;
}