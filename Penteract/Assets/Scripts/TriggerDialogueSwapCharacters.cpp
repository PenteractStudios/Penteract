#include "TriggerDialogueSwapCharacters.h"

#include "GameplaySystems.h"
#include "GameObject.h"

#include "DialogueManager.h"

EXPOSE_MEMBERS(TriggerDialogueSwapCharacters) {
    MEMBER(MemberType::GAME_OBJECT_UID, gameControllerUID)
};

GENERATE_BODY_IMPL(TriggerDialogueSwapCharacters);

void TriggerDialogueSwapCharacters::Start() {
    gameController = GameplaySystems::GetGameObject(gameControllerUID);
    if(gameController) dialogueManagerScript = GET_SCRIPT(gameController, DialogueManager);
}

void TriggerDialogueSwapCharacters::Update() {
	
}

void TriggerDialogueSwapCharacters::OnCollision(GameObject& /*collidedWith*/, float3 /*collisionNormal*/, float3 /*penetrationDistance*/, void* /*particle*/) {
    
    if (dialogueManagerScript) {
        dialogueManagerScript->SetActiveDialogue(&dialogueManagerScript->dialoguesArray[18]);
    }
    GetOwner().Disable();
}