#include "VideoScene1Start.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "PlayerController.h"
#include "GameController.h"
#include "CanvasFader.h"
#include "Components/UI/ComponentVideo.h"
#include "Components/UI/ComponentCanvas.h"

const char* VideoScene1Start::globalVariableKeyPlayVideoScene1 = "CameFromMainMenu";

EXPOSE_MEMBERS(VideoScene1Start) {
    MEMBER(MemberType::GAME_OBJECT_UID, canvasFaderUID)
};

GENERATE_BODY_IMPL(VideoScene1Start);

void VideoScene1Start::Start() {
    componentVideo = GetOwner().GetComponent<ComponentVideo>();
    parent = GetOwner().GetParent();

    if (canvasFaderUID > 0) {
        GameObject* canvasFaderObj = GameplaySystems::GetGameObject(canvasFaderUID);
        if (canvasFaderObj) {
            faderScript = GET_SCRIPT(canvasFaderObj, CanvasFader);
        }
    }

    componentVideo->SetVideoFrameSize(Screen::GetResolution().x, Screen::GetResolution().y);

    if (componentVideo) {
        if (GameplaySystems::GetGlobalVariable(globalVariableKeyPlayVideoScene1,true)) {
            componentVideo->Play();
            GameController::SetVideoActive(true);
            Time::PauseGame();
            GameplaySystems::SetGlobalVariable(globalVariableKeyPlayVideoScene1, false);
        } else {
            BackToNormalGameplay();
        }
    }
}

void VideoScene1Start::Update() {

    if ((componentVideo->HasVideoFinished() && componentVideo->IsActive()) || Input::GetKeyCodeDown(Input::KEYCODE::KEY_ESCAPE)) {
        BackToNormalGameplay();
    }
}

void VideoScene1Start::BackToNormalGameplay() {
    Time::ResumeGame();
    componentVideo->Stop();
    GameController::SetVideoActive(false);
    parent->Disable();
    if (faderScript) {
        faderScript->FadeIn();
    }
}
