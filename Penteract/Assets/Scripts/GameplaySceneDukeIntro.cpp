#include "GameplaySceneDukeIntro.h"

#include "GameplaySystems.h"
#include "GameController.h"

#include "Components/ComponentAgent.h"

EXPOSE_MEMBERS(GameplaySceneDukeIntro) {
    MEMBER(MemberType::GAME_OBJECT_UID, duke1UID),
    MEMBER(MemberType::GAME_OBJECT_UID, encounterPlazaUID)
};

GENERATE_BODY_IMPL(GameplaySceneDukeIntro);

void GameplaySceneDukeIntro::Start() {
    duke1 = GameplaySystems::GetGameObject(duke1UID);
    encounterPlaza = GameplaySystems::GetGameObject(encounterPlazaUID);

    dukeAgent = duke1->GetComponent<ComponentAgent>();
}

void GameplaySceneDukeIntro::Update() {
    if (GameController::IsGameplayBlocked()) return;

    // Make Duke move
    dukeAgent->SetMoveTarget(float3(350, 2.48, -1700));

    // Enable encounter 1

    // on finish:
    //GetOwner().Disable();
}