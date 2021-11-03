#include "DukeDeathLevel2Trigger.h"

#include "HUDManager.h"
#include "DialogueManager.h"
#include "GameplaySystems.h"
#include "GameObject.h"
#include "AfterDialogCallback.h"
#include "MovingLasers.h"
#include "GlobalVariables.h"
#include "AIDuke.h"

#include "GameObjectUtils.h"


EXPOSE_MEMBERS(DukeDeathLevel2Trigger) {
    MEMBER(MemberType::GAME_OBJECT_UID, dukeUID),
    MEMBER(MemberType::GAME_OBJECT_UID, gameControllerUID),
    MEMBER(MemberType::GAME_OBJECT_UID, laserUID),
    MEMBER(MemberType::GAME_OBJECT_UID, HUDUID),
    MEMBER(MemberType::GAME_OBJECT_UID, switchParticlesUID),
    MEMBER(MemberType::INT, dialogueID),
    MEMBER_SEPARATOR("Laser Doors"),
    MEMBER(MemberType::GAME_OBJECT_UID, entranceDoorUID),
    MEMBER(MemberType::GAME_OBJECT_UID, exitDoorUID),
    MEMBER_SEPARATOR("Dissolve material reference in placeholders"),
    MEMBER(MemberType::GAME_OBJECT_UID, doorDissolveUID)
};

GENERATE_BODY_IMPL(DukeDeathLevel2Trigger);

void DukeDeathLevel2Trigger::Start() {
    // Duke
    duke = GameplaySystems::GetGameObject(dukeUID);
    if (duke) aiDuke = GET_SCRIPT(duke, AIDuke);

    // Dialogue Mng Script
    gameController = GameplaySystems::GetGameObject(gameControllerUID);
    if (gameController) dialogueManagerScript = GET_SCRIPT(gameController, DialogueManager);

    // Moving Lasers Script
    GameObject* laser = GameplaySystems::GetGameObject(laserUID);
    laserScript = GET_SCRIPT(laser, MovingLasers);

    // Get Switch particle effect
    GameObject* switchParticlesGO = GameplaySystems::GetGameObject(switchParticlesUID);
    if (switchParticlesGO) switchParticles = switchParticlesGO->GetComponent<ComponentParticleSystem>();

    // Get Doors
    entraceDoor = GameplaySystems::GetGameObject(entranceDoorUID);
    exitDoor = GameplaySystems::GetGameObject(exitDoorUID);

    // Get dissolve material
    GameObject* dissolveObj = GameplaySystems::GetGameObject(doorDissolveUID);
    if (dissolveObj) {
        ComponentMeshRenderer* dissolveMeshRenderer = dissolveObj->GetComponent<ComponentMeshRenderer>();
        if (dissolveMeshRenderer) {
            doorDissolveMaterial = dissolveMeshRenderer->GetMaterial();
        }
    }

    // Scene flow controls
    triggered = false;
}

void DukeDeathLevel2Trigger::Update() {
    // After triggering the collider and finishing the dialogues
    if (triggered && !GameplaySystems::GetGlobalVariable(globalIsGameplayBlocked, true)) {
        // make Duke disappear
        if (laserScript) laserScript->TurnOff();
        if (aiDuke) {
            aiDuke->ActivateDissolve();
            if (switchParticles) switchParticles->PlayChildParticles();
        }

        // Unlock doors
        if (entraceDoor) entraceDoor->Disable();
        if (exitDoor) exitDoor->Disable();

        GetOwner().Disable();
        triggered = false;
    }
        //if (dukeMaterial && dukeMaterial->HasDissolveAnimationFinished())

}

void DukeDeathLevel2Trigger::OnCollision(GameObject& /*collidedWith*/, float3 /*collisionNormal*/, float3 /*penetrationDistance*/, void* /*particle*/) {
    // Show defeat dialogue
    if (dialogueManagerScript && !triggered) {
        if (dialogueID < sizeof(dialogueManagerScript->dialoguesArray) / sizeof(dialogueManagerScript->dialoguesArray[0])
            && &dialogueManagerScript->dialoguesArray[dialogueID] != nullptr) {
            dialogueManagerScript->PlayOpeningAudio();
            dialogueManagerScript->SetActiveDialogue(&dialogueManagerScript->dialoguesArray[dialogueID]);
            triggered = true;
        }
    }

    // Hide boss health
    GameObject* hudManagerGO = GameplaySystems::GetGameObject(HUDUID);
    if (hudManagerGO) {
        HUDManager* hudManager = GET_SCRIPT(hudManagerGO, HUDManager);
        if (hudManager) hudManager->HideBossHealth();
    }

    // Disable Trigger
    ComponentBoxCollider* collider = GetOwner().GetComponent<ComponentBoxCollider>();
    if (collider) {
        collider->Disable();
    }

    // Dissolve Doors
    PlayDissolveAnimation(entraceDoor, false);
    PlayDissolveAnimation(exitDoor, false);
}

// TODO: Almost duplicated code, copied from SpawnPointController
void DukeDeathLevel2Trigger::PlayDissolveAnimation(GameObject* root, bool playReverse) {
    if (!root) return;

    if (doorDissolveMaterial != 0) {
        GameObject* doorBack = GameObjectUtils::SearchReferenceInHierarchy(root, "DoorEnergyBack");
        if (doorBack) {
            ComponentMeshRenderer* meshRenderer = doorBack->GetComponent<ComponentMeshRenderer>();
            if (meshRenderer) {
                meshRenderer->SetMaterial(doorDissolveMaterial);
                meshRenderer->PlayDissolveAnimation(playReverse);
            }
        }

        GameObject* doorFront = GameObjectUtils::SearchReferenceInHierarchy(root, "DoorEnergyFront");
        if (doorFront) {
            ComponentMeshRenderer* meshRenderer = doorFront->GetComponent<ComponentMeshRenderer>();
            if (meshRenderer) {
                meshRenderer->SetMaterial(doorDissolveMaterial);
                meshRenderer->PlayDissolveAnimation(playReverse);
            }
        }
    }
}