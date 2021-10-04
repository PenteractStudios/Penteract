#include "GameplaySceneWalkToFactory.h"

#include "GameplaySystems.h"
#include "GameController.h"
#include "CameraController.h"
#include "DialogueManager.h"
#include "AIMovement.h"
#include "Components/ComponentAgent.h"

EXPOSE_MEMBERS(GameplaySceneWalkToFactory) {
    MEMBER_SEPARATOR("Duke Controller"),
    MEMBER(MemberType::GAME_OBJECT_UID, duke2UID),
    MEMBER(MemberType::FLOAT3, dukeRunTowards),
    MEMBER(MemberType::FLOAT, dukeSpeed),
    MEMBER_SEPARATOR("Camera Controller"),
    MEMBER(MemberType::GAME_OBJECT_UID, gameCameraUID),
    MEMBER(MemberType::FLOAT3, cameraNewPosition),
    MEMBER(MemberType::FLOAT, cameraPanningTime),
    MEMBER_SEPARATOR("Lasers"),
    MEMBER(MemberType::GAME_OBJECT_UID, laserBeamsSecurityUID),
    MEMBER_SEPARATOR("Final Dialogue"),
    MEMBER(MemberType::GAME_OBJECT_UID, gameControllerUID),
    MEMBER(MemberType::INT, dialogueID)
};

GENERATE_BODY_IMPL(GameplaySceneWalkToFactory);

void GameplaySceneWalkToFactory::Start() {
    // Duke
    // TODO: Duke is a placeholder for the real duke. The prefab or a part of it should be used instead to be congruent with the other duke instances in the game
    duke2 = GameplaySystems::GetGameObject(duke2UID);
    if (duke2) {
        movementScript = GET_SCRIPT(duke2, AIMovement);
        dukeAgent = duke2->GetComponent<ComponentAgent>();
        if (dukeAgent) {
            dukeAgent->SetMaxSpeed(dukeSpeed);
            dukeAgent->SetMaxAcceleration(AIMovement::maxAcceleration);
            dukeAgent->SetAgentObstacleAvoidance(true);
            dukeAgent->AddAgentToCrowd();
        }
    }

    // Camera
    gameCamera = GameplaySystems::GetGameObject(gameCameraUID);
    if (gameCamera) cameraControllerScript = GET_SCRIPT(gameCamera, CameraController);

    // Lasers
    laserBeamsSecurity = GameplaySystems::GetGameObject(laserBeamsSecurityUID);
    if (laserBeamsSecurity) laserBeamsSecurity->Disable();

    // Set up dialogue callback
    gameController = GameplaySystems::GetGameObject(gameControllerUID);
    if (gameController) dialogueManagerScript = GET_SCRIPT(gameController, DialogueManager);

    triggered = false;
    sceneStarted = false;
    finishScene = false;
}

void GameplaySceneWalkToFactory::Update() {
    if (triggered) {
        if (!duke2 || !dukeAgent || !cameraControllerScript || !dialogueManagerScript || !laserBeamsSecurity) return;

        GameController::BlockGameplay(true);
        cameraControllerScript->ChangeCameraOffset(cameraNewPosition.x, cameraNewPosition.y, cameraNewPosition.z);
        movementScript->Seek(state, dukeRunTowards, dukeAgent->GetMaxSpeed(), true);

        sceneStarted = true;
        triggered = false;
    }

    if (sceneStarted) {
        elapsedPanningTime += Time::GetDeltaTime();
        if (elapsedPanningTime >= cameraPanningTime) {
            finishScene = true;
            sceneStarted = false;
        }
    }

    if (finishScene) {
        cameraControllerScript->RestoreCameraOffset();
        GameController::BlockGameplay(false);
        dukeAgent->RemoveAgentFromCrowd();
        duke2->Disable();

        // When the scene finishes, open the dialogue directly
        if (dialogueManagerScript) {
            if (dialogueID < sizeof(dialogueManagerScript->dialoguesArray) / sizeof(dialogueManagerScript->dialoguesArray[0])
                && &dialogueManagerScript->dialoguesArray[dialogueID] != nullptr) {
                dialogueManagerScript->PlayOpeningAudio();
                dialogueManagerScript->SetActiveDialogue(&dialogueManagerScript->dialoguesArray[dialogueID]);
            }
        }

        laserBeamsSecurity->Enable();
        finishScene = false;
        GetOwner().Disable();
    }
}

void GameplaySceneWalkToFactory::OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle) {
    triggered = true;
}
