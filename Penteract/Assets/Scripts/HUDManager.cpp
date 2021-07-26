#include "HUDManager.h"

EXPOSE_MEMBERS(HUDManager) {
	MEMBER(MemberType::GAME_OBJECT_UID, fangSkillParentUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruSkillParentUID),
	MEMBER(MemberType::GAME_OBJECT_UID, switchSkillParentUID)
};

GENERATE_BODY_IMPL(HUDManager);

void HUDManager::Start() {

}

void HUDManager::Update() {

}