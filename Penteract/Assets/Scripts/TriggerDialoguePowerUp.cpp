#include "TriggerDialoguePowerUp.h"

#include "GameplaySystems.h"
#include "GameObject.h"
#include "GlobalVariables.h" 

#include "DialogueManager.h"
#include "PlayerController.h"

EXPOSE_MEMBERS(TriggerDialoguePowerUp) {
    MEMBER(MemberType::GAME_OBJECT_UID, gameControllerUID),
    MEMBER(MemberType::GAME_OBJECT_UID, playerUID),
    MEMBER(MemberType::INT, numUpgrade)
};

GENERATE_BODY_IMPL(TriggerDialoguePowerUp);

void TriggerDialoguePowerUp::Start() {
    gameController = GameplaySystems::GetGameObject(gameControllerUID);
    if (gameController) dialogueManagerScript = GET_SCRIPT(gameController, DialogueManager);
    player = GameplaySystems::GetGameObject(playerUID);
    if (player) playerControllerScript = GET_SCRIPT(player, PlayerController);
}

void TriggerDialoguePowerUp::Update() {

}

void TriggerDialoguePowerUp::OnCollision(GameObject& /*collidedWith*/, float3 /*collisionNormal*/, float3 /*penetrationDistance*/, void* /*particle*/) {

    if (GameplaySystems::GetGlobalVariable(globalLevel, 0) == 1) {
        switch (numUpgrade) {
        case 1:
            GameplaySystems::SetGlobalVariable(globalUpgradeLevel1_Plaza, true);
            break;        
        case 2:
            GameplaySystems::SetGlobalVariable(globalUpgradeLevel1_Cafeteria, true);
            break;        
        case 3:
            GameplaySystems::SetGlobalVariable(globalUpgradeLevel1_Presecurity, true);
            break;
        default:
            break;
        }
    }

    if (GameplaySystems::GetGlobalVariable(globalLevel, 0) == 2) {
        switch (numUpgrade) {
        case 1:
            GameplaySystems::SetGlobalVariable(globalUpgradeLevel2_Catwalks, true);
            break;
        case 2:
            GameplaySystems::SetGlobalVariable(globalUpgradeLevel2_AfterArena1, true);
            break;
        case 3:
            GameplaySystems::SetGlobalVariable(globalUpgradeLevel2_FireBridge, true);
            break;
        default:
            break;
        }
    }

    if (playerControllerScript) playerControllerScript->ObtainUpgradeCell();
    if (dialogueManagerScript) {
        dialogueManagerScript->PlayOpeningAudio();
        dialogueManagerScript->ActivatePowerUpDialogue();
    }
    GetOwner().Disable();
}