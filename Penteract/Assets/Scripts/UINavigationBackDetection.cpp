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
	if (objectToEnableOnCancelUID != 0) {
		objectToEnableOnCancel = GameplaySystems::GetGameObject(objectToEnableOnCancelUID);
	}

	if (objectToDisableOnCancelUID != 0) {
		objectToDisableOnCancel = GameplaySystems::GetGameObject(objectToDisableOnCancelUID);
	}

	if (objectToSelectOnEnableUID != 0) {
		GameObject* objectToSelectOnEnable = GameplaySystems::GetGameObject(objectToSelectOnEnableUID);
		if (objectToSelectOnEnable) {
			selectableToSelectOnEnable = objectToSelectOnEnable->GetComponent<ComponentSelectable>();
		}
	}

}

void UINavigationBackDetection::Update() {
	ListenForCancelInput();
	ManageNullSelection();
}

void UINavigationBackDetection::ManageNullSelection() {
	//if (selectOnUpdate) {
	//	selectOnUpdate = false;

	//	ComponentEventSystem* evSyst = UserInterface::GetCurrentEventSystem();
	//	if (evSyst && objectToSelectOnEnableUID != 0) {
	//		std::string message = "Tryna Select " + std::to_string(objectToSelectOnEnableUID);
	//		Debug::Log(message.c_str());
	//		evSyst->SetSelected(objectToSelectOnEnableUID);
	//	}
	//}
}

void UINavigationBackDetection::ListenForCancelInput() {
	if (Player::GetInputBool(InputActions::CANCEL_A, GameplaySystems::GetGlobalVariable<bool>(globalUseGamepad, true)) || Player::GetInputBool(InputActions::CANCEL_B, GameplaySystems::GetGlobalVariable<bool>(globalUseGamepad, true))) {
		OnCancelInputPressed();
	}
}

void UINavigationBackDetection::OnCancelInputPressed() {
	Debug::Log("CancelPressed");

	if (objectToDisableOnCancel) {
		objectToDisableOnCancel->Disable();
	}

	if (objectToEnableOnCancel) {
		objectToEnableOnCancel->Enable();
	}

}

void UINavigationBackDetection::OnEnable() {
	Debug::Log("OnEnable");
	ComponentEventSystem* evSyst = UserInterface::GetCurrentEventSystem();
	if (evSyst) {
		if (selectableToSelectOnEnable) {
			evSyst->firstSelectedId = selectableToSelectOnEnable->GetID();
			evSyst->SetSelected(selectableToSelectOnEnable->GetID());
			//selectOnUpdate = true;
			//Debug::Log((std::to_string(objectToSelectOnEnableUID) + " would be selected").c_str());
		}
	}
}