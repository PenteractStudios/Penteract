#include "UINavigationBackDetection.h"

#include "Components/UI/ComponentSelectable.h"
#include "GameplaySystems.h"
#include "GlobalVariables.h"
#include "Player.h"

EXPOSE_MEMBERS(UINavigationBackDetection) {
	MEMBER(MemberType::GAME_OBJECT_UID, objectToEnableOnCancelUID),
	MEMBER(MemberType::GAME_OBJECT_UID, objectToDisableOnCancelUID),
	MEMBER(MemberType::GAME_OBJECT_UID, objectToSelectOnEnableUID)
};

GENERATE_BODY_IMPL(UINavigationBackDetection);

void UINavigationBackDetection::Start() {
	objectToEnableOnCancel = GameplaySystems::GetGameObject(objectToEnableOnCancelUID);
	objectToDisableOnCancel = GameplaySystems::GetGameObject(objectToDisableOnCancelUID);
	GameObject* objectToSelectOnEnable = GameplaySystems::GetGameObject(objectToSelectOnEnableUID);
	if (objectToSelectOnEnable) {
		selectableToSelectOnEnable = objectToSelectOnEnable->GetComponent<ComponentSelectable>();
	}

}

void UINavigationBackDetection::Update() {
	ListenForCancelInput();
}


void UINavigationBackDetection::ListenForCancelInput() {
	if (Player::GetInputBool(InputActions::CANCEL_A) || Player::GetInputBool(InputActions::CANCEL_B)) {
		OnCancelInputPressed();
	}
}

void UINavigationBackDetection::OnCancelInputPressed() {
	if (objectToDisableOnCancel) {
		objectToDisableOnCancel->Disable();
	}

	if (objectToEnableOnCancel) {
		objectToEnableOnCancel->Enable();
	}

}

void UINavigationBackDetection::OnEnable() {
	ComponentEventSystem* evSyst = UserInterface::GetCurrentEventSystem();
	if (evSyst) {
		if (selectableToSelectOnEnable) {
			evSyst->firstSelectedId = selectableToSelectOnEnable->GetID();
			evSyst->SetSelected(selectableToSelectOnEnable->GetID());
		}
	}
}