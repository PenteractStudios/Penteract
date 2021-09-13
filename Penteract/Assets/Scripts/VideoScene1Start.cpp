#include "VideoScene1Start.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "PlayerController.h"
#include "GameController.h"
#include "Components/UI/ComponentVideo.h"
#include "Components/UI/ComponentCanvas.h"

EXPOSE_MEMBERS(VideoScene1Start) {
};

GENERATE_BODY_IMPL(VideoScene1Start);

void VideoScene1Start::Start() {
    componentVideo = GetOwner().GetComponent<ComponentVideo>();
    parent = GetOwner().GetParent();

    componentVideo->SetVideoFrameSize(Screen::GetResolution().x, Screen::GetResolution().y);
    if (componentVideo) {
        componentVideo->Play();
        GameController::SetVideoActive(true);
    }
    Time::PauseGame();
}

void VideoScene1Start::Update() {

    if ((componentVideo->HasVideoFinished() && componentVideo->IsActive()) || Input::GetKeyCodeDown(Input::KEYCODE::KEY_F12)) {
        Time::ResumeGame();
        componentVideo->Stop();
        GameController::SetVideoActive(false);
        parent->Disable();
    }
}