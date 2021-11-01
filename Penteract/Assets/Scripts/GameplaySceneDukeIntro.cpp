#include "GameplaySceneDukeIntro.h"

#include "GameplaySystems.h"
#include "GameController.h"
#include "AIMovement.h"
#include "Components/ComponentAgent.h"
#include "Components/ComponentAnimation.h"
#include "Components/UI/ComponentVideo.h"
#include "GlobalVariables.h"

EXPOSE_MEMBERS(GameplaySceneDukeIntro) {
    MEMBER_SEPARATOR("Object Refs"),
    MEMBER(MemberType::GAME_OBJECT_UID, duke1UID),
    MEMBER(MemberType::GAME_OBJECT_UID, playerUID),
    MEMBER(MemberType::GAME_OBJECT_UID, encounterPlazaUID),
    MEMBER(MemberType::GAME_OBJECT_UID, videoUID),
    MEMBER_SEPARATOR("Duke Controller"),
    MEMBER(MemberType::FLOAT3, dukeRunTowards),
    MEMBER(MemberType::FLOAT, dukeSpeed),
    MEMBER(MemberType::FLOAT, dukeTimeToGo),
    MEMBER(MemberType::FLOAT, dukeDisappearDistance)
};

GENERATE_BODY_IMPL(GameplaySceneDukeIntro);

void GameplaySceneDukeIntro::Start() {
    duke1 = GameplaySystems::GetGameObject(duke1UID);
    if (duke1) {
        dukeAnimation = duke1->GetComponent<ComponentAnimation>();
        movementScript = GET_SCRIPT(duke1, AIMovement);
        dukeAgent = duke1->GetComponent<ComponentAgent>();
        if (dukeAgent) {
            dukeAgent->SetMaxSpeed(dukeSpeed);
            dukeAgent->SetMaxAcceleration(AIMovement::maxAcceleration);
            dukeAgent->SetAgentObstacleAvoidance(true);
            dukeAgent->AddAgentToCrowd();
        }
    }

    player = GameplaySystems::GetGameObject(playerUID);
    encounterPlaza = GameplaySystems::GetGameObject(encounterPlazaUID);
    if (encounterPlaza) encounterPlaza->Disable();
    GameObject* videoObj = GameplaySystems::GetGameObject(videoUID);
    if (videoObj) videoIntro = videoObj->GetComponent<ComponentVideo>();
}

void GameplaySceneDukeIntro::Update() {
    if (!dukeAgent || !player || !videoIntro || !movementScript || !encounterPlaza) return;
    if (!videoIntro->HasVideoFinished()) return;
    if (GameplaySystems::GetGlobalVariable(globalIsGameplayBlocked, true)) return;

    // Make Duke move away
    Movement();
   

    // Enable encounter 1
    if (encounterPlaza && !encounterPlaza->IsActive()) encounterPlaza->Enable();

    // on finish:
    if (!movementScript->CharacterInSight(player, dukeDisappearDistance)) {
        dukeAgent->RemoveAgentFromCrowd();
        duke1->Disable();
        GetOwner().Disable();
    }
}

void GameplaySceneDukeIntro::Movement()
{
    currentDukeTimeToWalkAway += Time::GetDeltaTime();
    if (currentDukeTimeToWalkAway >= dukeTimeToGo) {
        movementScript->Seek(state, dukeRunTowards, dukeAgent->GetMaxSpeed(), true);
        if (dukeAnimation && dukeAnimation->GetCurrentState() && dukeAnimation->GetCurrentState()->name != "WalkForwardNoAim") {
            dukeAnimation->SendTrigger(dukeAnimation->GetCurrentState()->name + "WalkForwardNoAim");
        }
    } else {
        if (dukeAnimation && dukeAnimation->GetCurrentState() && dukeAnimation->GetCurrentState()->name != "PDA") {
            dukeAnimation->SendTrigger(dukeAnimation->GetCurrentState()->name + "PDA");
        }
    }
}
