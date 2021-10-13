#include "BossEncounterCallback.h"

#include "AIDuke.h"
#include "GameplaySystems.h"
#include "GameObject.h"

EXPOSE_MEMBERS(BossEncounterCallback) {
    MEMBER(MemberType::GAME_OBJECT_UID, dukeUID)
};

GENERATE_BODY_IMPL(BossEncounterCallback);

void BossEncounterCallback::Start() {
    gameObject = &GetOwner();
	duke = GameplaySystems::GetGameObject(dukeUID);
    if (duke) dukeScript = GET_SCRIPT(duke, AIDuke);
}

void BossEncounterCallback::OnEnable() {
    if (duke && dukeScript && shouldTeleport) {
        dukeScript->dukeCharacter.TeleportDuke(false);
        shouldTeleport = false;
    }
}

void BossEncounterCallback::OnDisable() {
    shouldTeleport = true;
}

void BossEncounterCallback::Update() {
    if (!shouldTeleport) {
        gameObject->Disable();
    }
}