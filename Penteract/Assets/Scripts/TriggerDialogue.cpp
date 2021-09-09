#include "TriggerDialogue.h"

#include "DialogueManager.h"
#include "GameplaySystems.h"
#include "GameObject.h"

EXPOSE_MEMBERS(TriggerDialogue) {
    MEMBER(MemberType::GAME_OBJECT_UID, gameControllerUID),
    MEMBER(MemberType::INT, dialogueID)
};

GENERATE_BODY_IMPL(TriggerDialogue);

void TriggerDialogue::Start() {
    gameController = GameplaySystems::GetGameObject(gameControllerUID);
    if (gameController) dialogueManagerScript = GET_SCRIPT(gameController, DialogueManager);
}

void TriggerDialogue::Update() {}
    
void TriggerDialogue::OnCollision(GameObject& /*collidedWith*/, float3 /*collisionNormal*/, float3 /*penetrationDistance*/, void* /*particle*/) {

    if (dialogueManagerScript) {
        if (dialogueID < sizeof(dialogueManagerScript->dialoguesArray) / sizeof(dialogueManagerScript->dialoguesArray[0])
            && &dialogueManagerScript->dialoguesArray[dialogueID] != nullptr) {
            dialogueManagerScript->SetActiveDialogue(&dialogueManagerScript->dialoguesArray[dialogueID]);
        }
    }
    GetOwner().Disable();
}