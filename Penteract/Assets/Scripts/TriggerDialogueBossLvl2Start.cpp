#include "TriggerDialogueBossLvl2Start.h"

#include "HUDManager.h"
#include "DialogueManager.h"
#include "GameplaySystems.h"
#include "GameObject.h"
#include "AfterDialogCallback.h"
#include "MovingLasers.h"
#include "GlobalVariables.h"
#include "AIDuke.h"


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
    if (boss) aiDuke = GET_SCRIPT(boss, AIDuke);
    gameController = GameplaySystems::GetGameObject(gameControllerUID);
    if (gameController) dialogueManagerScript = GET_SCRIPT(gameController, DialogueManager);

    GameObject* laser = GameplaySystems::GetGameObject(laserUID);
    laserScript = GET_SCRIPT(laser, MovingLasers);
}

void TriggerDialogueBossLvl2Start::Update() {
    if (triggered && !GameplaySystems::GetGlobalVariable(globalIsGameplayBlocked, true)) {
        if (SwitchOn) {
            if (laserScript) laserScript->TurnOn();
        } else {
            if (laserScript) laserScript->TurnOff();
            if (aiDuke) {
                aiDuke->ActivateDissolve();
                aiDuke->dukeCharacter.GetDukeMeshRenderer()->HasDissolveAnimationFinished(); // TODO: This does nothing here, but it's a reminder of the function that will be used later on for the scenes
            }
        }
        
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
    GameObject* hudManagerGO = GameplaySystems::GetGameObject(HUDUID);
    HUDManager* hudManager = nullptr;
    if (hudManagerGO) {
        hudManager = GET_SCRIPT(hudManagerGO, HUDManager);
        
    }
    if (SwitchOn) {
        boss->Enable();
        if (hudManager) hudManager->ShowBossHealth();
    }
    else {
        if (hudManager) hudManager->HideBossHealth();
    }
}