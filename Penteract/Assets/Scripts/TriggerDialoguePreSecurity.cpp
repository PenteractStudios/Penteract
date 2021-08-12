#include "TriggerDialoguePreSecurity.h"

#include "GameplaySystems.h"
#include "GameObject.h"

#include "DialogueManager.h"

EXPOSE_MEMBERS(TriggerDialoguePreSecurity) {
    MEMBER(MemberType::GAME_OBJECT_UID, gameControllerUID)
};

GENERATE_BODY_IMPL(TriggerDialoguePreSecurity);

void TriggerDialoguePreSecurity::Start() {
    gameController = GameplaySystems::GetGameObject(gameControllerUID);
    if (gameController) dialogueManagerScript = GET_SCRIPT(gameController, DialogueManager);
}

void TriggerDialoguePreSecurity::Update() {
	
}

void TriggerDialoguePreSecurity::OnCollision(GameObject& /*collidedWith*/, float3 /*collisionNormal*/, float3 /*penetrationDistance*/, void* /*particle*/) {

    if (dialogueManagerScript) {
        dialogueManagerScript->SetActiveDialogue(&dialogueManagerScript->dialoguesArray[26]);
    }
    GetOwner().Disable();
}