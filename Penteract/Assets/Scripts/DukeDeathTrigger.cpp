#include "DukeDeathTrigger.h"

#include "GameplaySystems.h"
#include "VideoSceneEnd.h"

#include "GlobalVariables.h"

EXPOSE_MEMBERS(DukeDeathTrigger) {
    MEMBER(MemberType::GAME_OBJECT_UID, videoCanvasUID)
};

GENERATE_BODY_IMPL(DukeDeathTrigger);

void DukeDeathTrigger::Start() {
    // Get videoObject
    videoCanvas = GameplaySystems::GetGameObject(videoCanvasUID);

    // Scene flow controls
    triggered = false;
    playVideo = false;
}

void DukeDeathTrigger::Update() {
    // more stuff
    if (triggered) {
        playVideo = true;
        triggered = false;
    }

    if (playVideo) {
        GameplaySystems::SetGlobalVariable(globalVariableKeyPlayVideoScene1, true);
        if (videoCanvas) {
            videoCanvas->GetParent()->Enable();
            VideoSceneEnd* videoSceneEndScript = GET_SCRIPT(videoCanvas, VideoSceneEnd);
            if (videoSceneEndScript) {
                videoSceneEndScript->PlayVideo();
            }

        }
    }
}

void DukeDeathTrigger::OnCollision(GameObject& /*collidedWith*/, float3 /*collisionNormal*/, float3 /*penetrationDistance*/, void* /*particle*/) {
    triggered = true;
}
