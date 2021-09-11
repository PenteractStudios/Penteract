#include "VideoScene1Start.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "PlayerController.h"
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
    }
    //Time::PauseGame(); // TODO Uncomment this line after release engine
}

void VideoScene1Start::Update() {

    if (componentVideo->HasVideoFinished() && componentVideo->IsActive()) {
        Time::ResumeGame();
        componentVideo->Stop();
        parent->Disable();
    }
}