#include "LaserTriggerLvl2.h"
#include "GameObject.h"
#include "GameplaySystems.h"
#include "MovingLasers.h"

EXPOSE_MEMBERS(LaserTriggerLvl2) {
    MEMBER(MemberType::GAME_OBJECT_UID, laserUID),
    MEMBER(MemberType::BOOL, SwitchOn)
};

GENERATE_BODY_IMPL(LaserTriggerLvl2);

void LaserTriggerLvl2::Start() {
    GameObject* laser = GameplaySystems::GetGameObject(laserUID);
    laserScript = GET_SCRIPT(laser, MovingLasers);
}

void LaserTriggerLvl2::Update() {
	
}

void LaserTriggerLvl2::OnCollision(GameObject& collidedWith, float3 /*collisionNormal*/, float3 /*penetrationDistance*/, void* /*particle*/) {
    if (collidedWith.name == "Fang" || collidedWith.name == "Onimaru") {
        if (laserScript) (SwitchOn)? laserScript->TurnOn() : laserScript->TurnOff();
        if (laserScript) laserScript->StopAudio();
        GameObject* ownerGo = &GetOwner();
        if(ownerGo) ownerGo->Disable();
    }
}