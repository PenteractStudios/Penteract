#include "DukeDoor.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "Components/Physics/ComponentBoxCollider.h"
#include "Components/ComponentObstacle.h"
#include "Components/ComponentAgent.h"

#include "AIDuke.h"
#include "PlayerController.h"
#include "HUDManager.h"
#include "DialogueManager.h"

#include "GlobalVariables.h"

EXPOSE_MEMBERS(DukeDoor) {
	MEMBER(MemberType::GAME_OBJECT_UID, playerUID),
    MEMBER(MemberType::GAME_OBJECT_UID, dukeUID),
    MEMBER(MemberType::GAME_OBJECT_UID, doorObstacleUID),
	MEMBER(MemberType::GAME_OBJECT_UID, canvasHUDUID),
	MEMBER(MemberType::GAME_OBJECT_UID, gameControllerUID),
	MEMBER(MemberType::INT, dialogueID),
	MEMBER_SEPARATOR("Scene Parameters"),
	MEMBER(MemberType::FLOAT3, initialTalkPosition)
};

GENERATE_BODY_IMPL(DukeDoor);

void DukeDoor::Start() {
	player = GameplaySystems::GetGameObject(playerUID);
	if (player) playerController = GET_SCRIPT(player, PlayerController);

	GameObject* duke = GameplaySystems::GetGameObject(dukeUID);
	if (duke) {
		aiDuke = GET_SCRIPT(duke, AIDuke);
		if (aiDuke) aiDuke->SetReady(false);
	}

	GameObject* obstacle = GameplaySystems::GetGameObject(doorObstacleUID);
	if (obstacle) {
		obstacle->Disable();
	}

	// Dialogue
	GameObject* gameController = GameplaySystems::GetGameObject(gameControllerUID);
	if (gameController) dialogueManagerScript = GET_SCRIPT(gameController, DialogueManager);

	GameplaySystems::SetGlobalVariable(globalMovePlayerFromCode, false);
	triggered = false;
	startDialogue = false;
}

void DukeDoor::Update() {

	// 1st part - Fang get to Duke
	if (triggered && player && playerController) {
		GameplaySystems::SetGlobalVariable(globalIsGameplayBlocked, true);
		GameplaySystems::SetGlobalVariable(globalMovePlayerFromCode, true);

		// Get the active character in the scene
		Player* playerReference;
		if (playerController->playerFang.IsActive()) {
			playerReference = &playerController->playerFang;
		}
		else {
			playerReference = &playerController->playerOnimaru;
		}

		// Move the character in front of Duke
		if (initialTalkPosition.Distance(playerReference->playerMainTransform->GetGlobalPosition()) > 0.5f) {
			playerReference->MoveTo(initialTalkPosition);
		}
		else {
			triggered = false;
			startDialogue = true;
			GameplaySystems::SetGlobalVariable(globalMovePlayerFromCode, false);
		}
	}

	// 2nd part - Dialogue
	if (startDialogue && dialogueManagerScript) {
		if (dialogueID < sizeof(dialogueManagerScript->dialoguesArray) / sizeof(dialogueManagerScript->dialoguesArray[0])
			&& &dialogueManagerScript->dialoguesArray[dialogueID] != nullptr) {
			dialogueManagerScript->PlayOpeningAudio();
			dialogueManagerScript->SetActiveDialogue(&dialogueManagerScript->dialoguesArray[dialogueID]);
		}
	}
}

void DukeDoor::OnCollision(GameObject& /*collidedWith*/, float3 /*collisionNormal*/ , float3 /*penetrationDistance*/ , void* /*particle*/ ) {


		ComponentBoxCollider* collider = GetOwner().GetComponent<ComponentBoxCollider>();
		if (collider) {
			collider->Disable();
		}

		GameObject* obstacle = GameplaySystems::GetGameObject(doorObstacleUID);
		if (obstacle) {
			obstacle->Enable();
		}


		GameObject* hudObj = GameplaySystems::GetGameObject(canvasHUDUID);
		if (hudObj) {

			HUDManager* hudMng = GET_SCRIPT(hudObj, HUDManager);

			if (hudMng) {
				hudMng->ShowBossHealth();
			}
		}

		triggered = true;

		if (aiDuke) aiDuke->SetReady(true);
	}
}