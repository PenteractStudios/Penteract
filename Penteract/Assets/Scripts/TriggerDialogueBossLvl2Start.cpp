#include "TriggerDialogueBossLvl2Start.h"

#include "DialogueManager.h"
#include "GameplaySystems.h"
#include "GameObject.h"
#include "AfterDialogCallback.h"
#include "MovingLasers.h"
#include "GlobalVariables.h"

EXPOSE_MEMBERS(TriggerDialogueBossLvl2Start) {
    MEMBER(MemberType::GAME_OBJECT_UID, BossUID),
    MEMBER(MemberType::GAME_OBJECT_UID, gameControllerUID),     
    MEMBER(MemberType::GAME_OBJECT_UID, laserUID),
    MEMBER(MemberType::INT, dialogueID),
    MEMBER(MemberType::BOOL, SwitchOn)
};

GENERATE_BODY_IMPL(TriggerDialogueBossLvl2Start);

void TriggerDialogueBossLvl2Start::Start() {
    boss = GameplaySystems::GetGameObject(BossUID);
    gameController = GameplaySystems::GetGameObject(gameControllerUID);
    if (gameController) dialogueManagerScript = GET_SCRIPT(gameController, DialogueManager);

    GameObject* laser = GameplaySystems::GetGameObject(laserUID);
    laserScript = GET_SCRIPT(laser, MovingLasers);
}

void TriggerDialogueBossLvl2Start::Update() {
    if (triggered && !GameplaySystems::GetGlobalVariable(globalIsGameplayBlocked, true)) {
        if (laserScript) (SwitchOn) ? laserScript->TurnOn() : laserScript->TurnOff();
        GetOwner().Disable();
        triggered = false;
    }
}

void TriggerDialogueBossLvl2Start::OnCollision(GameObject& /*collidedWith*/, float3 /*collisionNormal*/, float3 /*penetrationDistance*/, void* /*particle*/) {

    if (dialogueManagerScript && !triggered) {
        if (dialogueID < sizeof(dialogueManagerScript->dialoguesArray) / sizeof(dialogueManagerScript->dialoguesArray[0])
            && &dialogueManagerScript->dialoguesArray[dialogueID] != nullptr) {
            dialogueManagerScript->PlayOpeningAudio();
            dialogueManagerScript->SetActiveDialogue(&dialogueManagerScript->dialoguesArray[dialogueID]);
            triggered = true;
        }
    }
    boss->Enable();
}