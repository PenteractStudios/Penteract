#include "VideoScene1Start.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "PlayerController.h"
#include "GameController.h"
#include "DialogueManager.h"
#include "CanvasFader.h"
#include "Components/UI/ComponentVideo.h"
#include "Components/UI/ComponentCanvas.h"
#include "GlobalVariables.h" 

EXPOSE_MEMBERS(VideoScene1Start) {
    MEMBER(MemberType::GAME_OBJECT_UID, canvasFaderUID),
    MEMBER(MemberType::GAME_OBJECT_UID, gameControllerUID),
    MEMBER(MemberType::INT, dialogueID)
};

GENERATE_BODY_IMPL(VideoScene1Start);

void VideoScene1Start::Start() {
    // Set up dialogue callback
    gameController = GameplaySystems::GetGameObject(gameControllerUID);
    if (gameController) dialogueManagerScript = GET_SCRIPT(gameController, DialogueManager);

    // Set up video
    componentVideo = GetOwner().GetComponent<ComponentVideo>();
    parent = GetOwner().GetParent();

    if (canvasFaderUID > 0) {
        GameObject* canvasFaderObj = GameplaySystems::GetGameObject(canvasFaderUID);
        if (canvasFaderObj) {
            faderScript = GET_SCRIPT(canvasFaderObj, CanvasFader);
        }
    }

    componentVideo->SetVideoFrameSize(static_cast<int>(Screen::GetResolution().x), static_cast<int>(Screen::GetResolution().y));

    if (componentVideo) {
        if (GameplaySystems::GetGlobalVariable(globalVariableKeyPlayVideoScene1,true)) {
            componentVideo->Play();
            GameplaySystems::SetGlobalVariable(isVideoActive, true);
            Time::PauseGame();
            GameplaySystems::SetGlobalVariable(globalVariableKeyPlayVideoScene1, false);
        } else {
            BackToNormalGameplay();
        }
    }

    GameplaySystems::SetGlobalVariable(globalIsGameplayBlocked, true);
}

void VideoScene1Start::Update() {

    if ((componentVideo->HasVideoFinished() && componentVideo->IsActive()) || Input::GetKeyCodeDown(Input::KEYCODE::KEY_ESCAPE)) {
        BackToNormalGameplay();
    }
}

void VideoScene1Start::BackToNormalGameplay() {
    Time::ResumeGame();
    componentVideo->Stop();
    GameplaySystems::SetGlobalVariable(isVideoActive, false);
    parent->Disable();
    if (faderScript) {
        faderScript->FadeIn();
    }

    GameplaySystems::SetGlobalVariable(globalIsGameplayBlocked, false);
  
    // When the video finishes, open the initial dialogue directly
    if (dialogueManagerScript) {
        if (dialogueID < sizeof(dialogueManagerScript->dialoguesArray) / sizeof(dialogueManagerScript->dialoguesArray[0])
            && &dialogueManagerScript->dialoguesArray[dialogueID] != nullptr) {
            dialogueManagerScript->PlayOpeningAudio();
            dialogueManagerScript->SetActiveDialogue(&dialogueManagerScript->dialoguesArray[dialogueID]);
        }
    }
  
}
