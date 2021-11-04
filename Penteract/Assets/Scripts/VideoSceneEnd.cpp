#include "VideoSceneEnd.h"
#include "GameObject.h"
#include "GameplaySystems.h"
#include "GlobalVariables.h"
#include "Components/UI/ComponentVideo.h"

EXPOSE_MEMBERS(VideoSceneEnd) {
    MEMBER(MemberType::SCENE_RESOURCE_UID, winSceneUID),

};

GENERATE_BODY_IMPL(VideoSceneEnd);

void VideoSceneEnd::Start() {
    // Set up video
    componentVideo = GetOwner().GetComponent<ComponentVideo>();
    if (componentVideo) componentVideo->SetVideoFrameSize(static_cast<int>(Screen::GetResolution().x), static_cast<int>(Screen::GetResolution().y));
    parent = GetOwner().GetParent();

}

void VideoSceneEnd::Update() {

    if (componentVideo && ((componentVideo->HasVideoFinished() && componentVideo->IsActive()) || Input::GetKeyCodeDown(Input::KEYCODE::KEY_ESCAPE))) {
        BackToNormalGameplay();
        if (winSceneUID != 0) {
            SceneManager::ChangeScene(winSceneUID);
        }

    }
}

void VideoSceneEnd::PlayVideo()
{
    if (componentVideo) {
        if (GameplaySystems::GetGlobalVariable(globalVariableKeyPlayVideoScene1, true)) {
            componentVideo->Play();
            GameplaySystems::SetGlobalVariable(isVideoActive, true);
            GameplaySystems::SetGlobalVariable(globalVariableKeyPlayVideoScene1, false);
        }
        else {
            BackToNormalGameplay();
        }
    }

    GameplaySystems::SetGlobalVariable(globalIsGameplayBlocked, true);
}

void VideoSceneEnd::BackToNormalGameplay()
{
    componentVideo->Stop();
    GameplaySystems::SetGlobalVariable(isVideoActive, false);

    GameplaySystems::SetGlobalVariable(globalIsGameplayBlocked, false);
}
