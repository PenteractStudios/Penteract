#include "TriggerDialogueBossLvl2Start.h"

#include "HUDManager.h"
#include "DialogueManager.h"
#include "GameplaySystems.h"
#include "GameObject.h"
#include "AfterDialogCallback.h"
#include "MovingLasers.h"
#include "AIDuke.h"
#include "GlobalVariables.h"


EXPOSE_MEMBERS(TriggerDialogueBossLvl2Start) {
    MEMBER(MemberType::GAME_OBJECT_UID, BossUID),
    MEMBER(MemberType::GAME_OBJECT_UID, gameControllerUID),     
    MEMBER(MemberType::GAME_OBJECT_UID, laserUID),
    MEMBER(MemberType::GAME_OBJECT_UID, HUDUID),
    MEMBER(MemberType::INT, dialogueID),
    MEMBER(MemberType::BOOL, SwitchOn)
};

GENERATE_BODY_IMPL(TriggerDialogueBossLvl2Start);

void TriggerDialogueBossLvl2Start::Start() {
    boss = GameplaySystems::GetGameObject(BossUID);
    if (boss) {
        boss->Enable();

        aiDuke = GET_SCRIPT(boss, AIDuke);
        if (aiDuke) aiDuke->SetReady(false);
    }

    gameController = GameplaySystems::GetGameObject(gameControllerUID);
    if (gameController) dialogueManagerScript = GET_SCRIPT(gameController, DialogueManager);

    laserScript = GET_SCRIPT(GameplaySystems::GetGameObject(laserUID), MovingLasers);
}

void TriggerDialogueBossLvl2Start::Update() {
    if (triggered) {
        if (SwitchOn) {
            if (!GameplaySystems::GetGlobalVariable(globalIsGameplayBlocked, true)) {
                laserScript->TurnOn();
                GetOwner().Disable();
                triggered = false;
            }
        } else {
            laserScript->TurnOff();
            //play duke death animation
            if (!GameplaySystems::GetGlobalVariable(globalIsGameplayBlocked, true)) {
                // perform duke get away
                // if (got away) {
                boss->Disable();
                GetOwner().Disable();
                triggered = false;
                // }
            }
        }

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
    GameObject* hudManagerGO = GameplaySystems::GetGameObject(HUDUID);
    HUDManager* hudManager = nullptr;
    if (hudManagerGO) {
        hudManager = GET_SCRIPT(hudManagerGO, HUDManager);
        
    }
    if (SwitchOn) {
        if (hudManager) hudManager->ShowBossHealth();
        if (aiDuke) aiDuke->SetReady(true);
    }
    else {
        if (hudManager) hudManager->HideBossHealth();
    }
}