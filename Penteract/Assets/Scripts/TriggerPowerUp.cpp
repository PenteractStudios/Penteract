#include "TriggerPowerUp.h"

#include "GameplaySystems.h"
#include "GameObject.h"

#include "DialogueManager.h"
#include "PlayerController.h"

EXPOSE_MEMBERS(TriggerPowerUp) {
    MEMBER(MemberType::GAME_OBJECT_UID, gameControllerUID),
    MEMBER(MemberType::GAME_OBJECT_UID, playerUID)
};

GENERATE_BODY_IMPL(TriggerPowerUp);

void TriggerPowerUp::Start() {
    gameController = GameplaySystems::GetGameObject(gameControllerUID);
    if (gameController) dialogueManagerScript = GET_SCRIPT(gameController, DialogueManager);
    player = GameplaySystems::GetGameObject(playerUID);
    if (player) playerControllerScript = GET_SCRIPT(player, PlayerController);
}

void TriggerPowerUp::Update() {

}

void TriggerPowerUp::OnCollision(GameObject& /*collidedWith*/, float3 /*collisionNormal*/, float3 /*penetrationDistance*/, void* /*particle*/) {

    if (playerControllerScript) playerControllerScript->ObtainUpgradeCell();
    if (dialogueManagerScript) dialogueManagerScript->ActivatePowerUpDialogue();
    GetOwner().Disable();
}