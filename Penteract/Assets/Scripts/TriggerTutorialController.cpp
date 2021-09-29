#include "TriggerTutorialController.h"

#include "GameplaySystems.h"
#include "GameObject.h"

EXPOSE_MEMBERS(TriggerTutorialController) {
    MEMBER(MemberType::GAME_OBJECT_UID, tutorialFang1UID),
	MEMBER(MemberType::GAME_OBJECT_UID, tutorialFang2UID),
	MEMBER(MemberType::GAME_OBJECT_UID, tutorialFang3UID),
	MEMBER(MemberType::GAME_OBJECT_UID, tutorialFangUltiUID),
	MEMBER(MemberType::GAME_OBJECT_UID, tutorialOnimaru1UID),
	MEMBER(MemberType::GAME_OBJECT_UID, tutorialOnimaru2UID),
	MEMBER(MemberType::GAME_OBJECT_UID, tutorialOnimaruUltiUID),
};

GENERATE_BODY_IMPL(TriggerTutorialController);

void TriggerTutorialController::Start() {
	// This is only used when restarting scene and such, as these gameobjects have to be disabled on Start for the game to function correctly
	tutorialFang1 = GameplaySystems::GetGameObject(tutorialFang1UID);
	tutorialFang2 = GameplaySystems::GetGameObject(tutorialFang2UID);
	tutorialFang3 = GameplaySystems::GetGameObject(tutorialFang3UID);
	tutorialFangUlti = GameplaySystems::GetGameObject(tutorialFangUltiUID);
	tutorialOnimaru1 = GameplaySystems::GetGameObject(tutorialOnimaru1UID);
	tutorialOnimaru2 = GameplaySystems::GetGameObject(tutorialOnimaru2UID);
	tutorialOnimaruUlti = GameplaySystems::GetGameObject(tutorialOnimaruUltiUID);

	if (tutorialFang2 && tutorialFang2->IsActive()) tutorialFang2->Disable();
	if (tutorialFang3 && tutorialFang3->IsActive()) tutorialFang3->Disable();
	if (tutorialFangUlti && tutorialFangUlti->IsActive()) tutorialFangUlti->Disable();
	if (tutorialOnimaru1 && tutorialOnimaru1->IsActive()) tutorialOnimaru1->Disable();
	if (tutorialOnimaru2 && tutorialOnimaru2->IsActive()) tutorialOnimaru2->Disable();
	if (tutorialOnimaruUlti && tutorialOnimaruUlti->IsActive()) tutorialOnimaruUlti->Disable();
}

void TriggerTutorialController::Update() {}