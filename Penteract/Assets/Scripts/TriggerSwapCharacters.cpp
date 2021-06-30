#include "TriggerSwapCharacters.h"

#include "GameplaySystems.h"
#include "GameObject.h"

#include "DialogueManager.h"

EXPOSE_MEMBERS(TriggerSwapCharacters) {
    MEMBER(MemberType::GAME_OBJECT_UID, gameControllerUID)
    // Add members here to expose them to the engine. Example:
    // MEMBER(MemberType::BOOL, exampleMember1),
    // MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
    // MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
};

GENERATE_BODY_IMPL(TriggerSwapCharacters);

void TriggerSwapCharacters::Start() {
    gameController = GameplaySystems::GetGameObject(gameControllerUID);
    if(gameController) dialogueManagerScript = GET_SCRIPT(gameController, DialogueManager);
}

void TriggerSwapCharacters::Update() {
	
}

void TriggerSwapCharacters::OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance) {
    
    if (dialogueManagerScript) {
        dialogueManagerScript->SetActiveDialogue(&dialogueManagerScript->dialoguesArray[0]);
    }
    GetOwner().Disable();
}