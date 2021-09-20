#include "GameplaySceneDukeIntro.h"

#include "GameplaySystems.h"
#include "GameController.h"
#include "AIMovement.h"
#include "Components/ComponentAgent.h"
#include "Components/UI/ComponentVideo.h"

EXPOSE_MEMBERS(GameplaySceneDukeIntro) {
    MEMBER(MemberType::GAME_OBJECT_UID, duke1UID),
    MEMBER(MemberType::GAME_OBJECT_UID, playerUID),
    MEMBER(MemberType::GAME_OBJECT_UID, encounterPlazaUID),
    MEMBER(MemberType::GAME_OBJECT_UID, videoUID)
};

GENERATE_BODY_IMPL(GameplaySceneDukeIntro);

void GameplaySceneDukeIntro::Start() {
    duke1 = GameplaySystems::GetGameObject(duke1UID);
    movementScript = GET_SCRIPT(duke1, AIMovement);
    player = GameplaySystems::GetGameObject(playerUID);
    encounterPlaza = GameplaySystems::GetGameObject(encounterPlazaUID);
    if (encounterPlaza) encounterPlaza->Disable();
    GameObject* videoObj = GameplaySystems::GetGameObject(videoUID);
    if (videoObj) videoIntro = videoObj->GetComponent<ComponentVideo>();


    dukeAgent = duke1->GetComponent<ComponentAgent>();
    if (dukeAgent) {
        dukeAgent->SetMaxSpeed(3.0f); // TODO: this should be taken from Duke AI script
        dukeAgent->SetMaxAcceleration(9999);
        dukeAgent->SetAgentObstacleAvoidance(true);
        dukeAgent->AddAgentToCrowd();
    }
}

void GameplaySceneDukeIntro::Update() {
    if (!dukeAgent || !player || !videoIntro || !movementScript || !encounterPlaza) return;
    if (!videoIntro->HasVideoFinished()) return;
    if (GameController::IsGameplayBlocked()) return;

    // Make Duke move away
    movementScript->Seek(state, float3(-70.f, 3.f, 0.f), dukeAgent->GetMaxSpeed(), true); // TODO: magic number for final position

    // Enable encounter 1
    if (encounterPlaza && !encounterPlaza->IsActive()) encounterPlaza->Enable();

    // on finish:
    if (!movementScript->CharacterInSight(player, 45.f)) { // TODO: magic number for searchDistance
        dukeAgent->RemoveAgentFromCrowd();
        duke1->Disable();
        GetOwner().Disable();
    }
}