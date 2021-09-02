#include "StartTitleGlitchOnPlay.h"
#include "GlitchyTitleController.h"
#include "GameplaySystems.h"
EXPOSE_MEMBERS(StartTitleGlitchOnPlay) {
	MEMBER(MemberType::GAME_OBJECT_UID, controllerObjUID)
};

GENERATE_BODY_IMPL(StartTitleGlitchOnPlay);

void StartTitleGlitchOnPlay::Start() {
	GameObject* controllerObj = GameplaySystems::GetGameObject(controllerObjUID);
	if (controllerObj) {
		controller = GET_SCRIPT(controllerObj, GlitchyTitleController);
	}
}

void StartTitleGlitchOnPlay::Update() {

}

void StartTitleGlitchOnPlay::OnButtonClick() {
	if (controller) {
		controller->PressedPlay();
	}
}
