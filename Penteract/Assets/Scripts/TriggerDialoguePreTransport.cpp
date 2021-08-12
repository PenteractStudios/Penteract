#include "TriggerDialoguePreTransport.h"

#include "GameplaySystems.h"
#include "GameObject.h"

#include "DialogueManager.h"

EXPOSE_MEMBERS(TriggerDialoguePreTransport) {
	MEMBER(MemberType::GAME_OBJECT_UID, gameControllerUID)
};

GENERATE_BODY_IMPL(TriggerDialoguePreTransport);

void TriggerDialoguePreTransport::Start() {
	gameController = GameplaySystems::GetGameObject(gameControllerUID);
	if (gameController) dialogueManagerScript = GET_SCRIPT(gameController, DialogueManager);
}

void TriggerDialoguePreTransport::Update() {
}

void TriggerDialoguePreTransport::OnCollision(GameObject& /*collidedWith*/, float3 /*collisionNormal*/, float3 /*penetrationDistance*/, void* /*particle*/) {

    if (dialogueManagerScript) {
        dialogueManagerScript->SetActiveDialogue(&dialogueManagerScript->dialoguesArray[24]);
    }
    GetOwner().Disable();
}