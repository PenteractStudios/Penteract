#include "TriggerDialogueFinalLvl1.h"

#include "GameplaySystems.h"
#include "GameObject.h"

#include "DialogueManager.h"

EXPOSE_MEMBERS(TriggerDialogueFinalLvl1) {
    MEMBER(MemberType::GAME_OBJECT_UID, gameControllerUID)
};

GENERATE_BODY_IMPL(TriggerDialogueFinalLvl1);

void TriggerDialogueFinalLvl1::Start() {
    gameController = GameplaySystems::GetGameObject(gameControllerUID);
    if (gameController) dialogueManagerScript = GET_SCRIPT(gameController, DialogueManager);
}

void TriggerDialogueFinalLvl1::Update() {

}

void TriggerDialogueFinalLvl1::OnCollision(GameObject& /*collidedWith*/, float3 /*collisionNormal*/, float3 /*penetrationDistance*/, void* /*particle*/) {

    if (dialogueManagerScript) {
        dialogueManagerScript->SetActiveDialogue(&dialogueManagerScript->dialoguesArray[28]);
    }
    GetOwner().Disable();
}