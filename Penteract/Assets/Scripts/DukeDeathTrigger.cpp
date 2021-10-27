#include "DukeDeathTrigger.h"

#include "GameplaySystems.h"
#include "PlayerController.h"
#include "DialogueManager.h"
#include "CameraController.h"
#include "HUDManager.h"
#include "VideoSceneEnd.h"

#include "GlobalVariables.h"

EXPOSE_MEMBERS(DukeDeathTrigger) {
    MEMBER(MemberType::GAME_OBJECT_UID, playerUID),
    MEMBER(MemberType::GAME_OBJECT_UID, dukeUID),
    MEMBER(MemberType::GAME_OBJECT_UID, gameControllerUID),
    MEMBER(MemberType::GAME_OBJECT_UID, gameCameraUID),
    MEMBER(MemberType::GAME_OBJECT_UID, canvasHudUID),
    MEMBER(MemberType::GAME_OBJECT_UID, videoCanvasUID),
    MEMBER(MemberType::FLOAT, relaxTime),
    MEMBER(MemberType::FLOAT, talkingDistance),
    MEMBER(MemberType::INT, dialogueID)
};

GENERATE_BODY_IMPL(DukeDeathTrigger);

void DukeDeathTrigger::Start() {
    // Get Player
    GameObject* player = GameplaySystems::GetGameObject(playerUID);
    if (player) playerController = GET_SCRIPT(player, PlayerController);

    // Get Duke
    duke = GameplaySystems::GetGameObject(dukeUID);

    // Get Dialogues
    GameObject* gameController = GameplaySystems::GetGameObject(gameControllerUID);
    if (gameController) dialogueManagerScript = GET_SCRIPT(gameController, DialogueManager);

    // Get Camera
    GameObject* cameraObj = GameplaySystems::GetGameObject(gameCameraUID);
    if (cameraObj) camera = GET_SCRIPT(cameraObj, CameraController);

    // Get videoObject
    videoCanvas = GameplaySystems::GetGameObject(videoCanvasUID);

    // Scene flow controls
    triggered = false;
    sceneStart = false;
    startDialogue = false;
    playVideo = false;
}

void DukeDeathTrigger::Update() {
    if (!camera || !dialogueManagerScript || !playerController || !duke) return;

    // 0st part - wait a few breathing seconds
    if (triggered) {
        if (elapsedRelaxTime < relaxTime) elapsedRelaxTime += Time::GetDeltaTime();
        else {
            triggered = false;
            sceneStart = true;
        }
    }

    // 1st part - Fang get to Duke
    if (sceneStart) {

        // Hide Boss health
        GameObject* hudObj = GameplaySystems::GetGameObject(canvasHudUID);
        if (hudObj) {
            HUDManager* hudMng = GET_SCRIPT(hudObj, HUDManager);
            if (hudMng) hudMng->HideBossHealth();
        }

        GameplaySystems::SetGlobalVariable(globalIsGameplayBlocked, true);
        GameplaySystems::SetGlobalVariable(globalMovePlayerFromCode, true);

        // Zoom out camera pre-dialogues
        if (camera && dialogueManagerScript) camera->ChangeCameraOffset(dialogueManagerScript->zoomOutCameraPosition.x, dialogueManagerScript->zoomOutCameraPosition.y, dialogueManagerScript->zoomOutCameraPosition.z);

        // Get the active character in the scene
        Player* playerReference;
        if (playerController->playerFang.IsActive()) playerReference = &playerController->playerFang;
        else playerReference = &playerController->playerOnimaru;

        // Get Duke in-front position
        ComponentTransform* dukeTransform = duke->GetComponent<ComponentTransform>();
        if (dukeTransform) talkPosition = dukeTransform->GetGlobalPosition() + talkingDistance * dukeTransform->GetFront();
            

        // Move the character in front of Duke
        if (talkPosition.Distance(playerReference->playerMainTransform->GetGlobalPosition()) > 0.5f) playerReference->MoveTo(talkPosition);
        else {
            sceneStart = false;
            startDialogue = true;
            GameplaySystems::SetGlobalVariable(globalMovePlayerFromCode, false);
        }
    }

    // 2nd part - Dialogue
    if (startDialogue) {
        if (dialogueID < sizeof(dialogueManagerScript->dialoguesArray) / sizeof(dialogueManagerScript->dialoguesArray[0])
            && &dialogueManagerScript->dialoguesArray[dialogueID] != nullptr) {
            dialogueManagerScript->PlayOpeningAudio();
            dialogueManagerScript->SetActiveDialogue(&dialogueManagerScript->dialoguesArray[dialogueID]);
        }
        startDialogue = false;
        playVideo = true;
    }

    // 3rd part - Ending Video  (waits for dialogue close with globalIsGameplayBlocked)
    if (playVideo && !GameplaySystems::GetGlobalVariable(globalIsGameplayBlocked, true)) {
        if (videoCanvas) {
            GameplaySystems::SetGlobalVariable(globalVariableKeyPlayVideoScene1, true);
            videoCanvas->GetParent()->Enable();
            videoSceneEndScript = GET_SCRIPT(videoCanvas, VideoSceneEnd);
            if (videoSceneEndScript) {
                videoSceneEndScript->PlayVideo();
            }

        }
    }
}

void DukeDeathTrigger::OnCollision(GameObject& /*collidedWith*/, float3 /*collisionNormal*/, float3 /*penetrationDistance*/, void* /*particle*/) {

    // Disable Trigger
    ComponentBoxCollider* collider = GetOwner().GetComponent<ComponentBoxCollider>();
    if (collider) {
        collider->Disable();
    }

    triggered = true;
}
