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
    Debug::Log("Start");
}

void BossEncounterCallback::Init() {
    Debug::Log("Init");
}

void BossEncounterCallback::Update() {
	if (duke && dukeScript) {
        if(shouldTeleport) {
            dukeScript->TeleportDuke();
            shouldTeleport = false;
        } else {
            gameObject->Disable();
        }
    }
}