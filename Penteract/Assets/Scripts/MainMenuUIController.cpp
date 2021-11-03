#include "MainMenuUIController.h"

#include "GameplaySystems.h"
#include "GlobalVariables.h"
#include "GameObject.h"
#include "Components/UI/ComponentImage.h"

EXPOSE_MEMBERS(MainMenuUIController) {
	MEMBER(MemberType::GAME_OBJECT_UID, leftFaderObjUID),
	MEMBER(MemberType::GAME_OBJECT_UID, rightFaderObjUID),
	MEMBER(MemberType::FLOAT, minFadeAlpha),
	MEMBER(MemberType::FLOAT, maxFadeAlpha)
};

GENERATE_BODY_IMPL(MainMenuUIController);

void MainMenuUIController::Start() {
	GameObject* leftFaderObj = GameplaySystems::GetGameObject(leftFaderObjUID);
	if (leftFaderObj) {
		leftFaderImage = leftFaderObj->GetComponent<ComponentImage>();
	}
	GameObject* rightFaderObj = GameplaySystems::GetGameObject(rightFaderObjUID);

	if (rightFaderObj) {
		rightFaderImage = rightFaderObj->GetComponent<ComponentImage>();
	}

}

void MainMenuUIController::Update() {

	if (Input::GetKeyCodeDown(Input::KEY_KP_PLUS)) {
		GameplaySystems::SetGlobalVariable<bool>(globalUseGamepad, !GameplaySystems::GetGlobalVariable<bool>(globalUseGamepad, true));

	}

	if (!rightFaderImage || !leftFaderImage) return;

	float4 color = rightFaderImage->GetColor();
	if (fadeLoopTimer < (fadeLoopDuration / 2)) {
		float delta = fadeLoopTimer / (fadeLoopDuration / 2);
		rightFaderImage->SetColor(float4(color.xyz(), Lerp(minFadeAlpha, maxFadeAlpha, delta)));
		leftFaderImage->SetColor(float4(color.xyz(), Lerp(minFadeAlpha, maxFadeAlpha, delta)));

	} else {
		float delta = (fadeLoopTimer - (fadeLoopDuration / 2)) / (fadeLoopDuration / 2);
		rightFaderImage->SetColor(float4(color.xyz(), Lerp(maxFadeAlpha, minFadeAlpha, delta)));
		leftFaderImage->SetColor(float4(color.xyz(), Lerp(maxFadeAlpha, minFadeAlpha, delta)));

	}

	fadeLoopTimer += Time::GetDeltaTime();
	if (fadeLoopTimer > fadeLoopDuration) {
		fadeLoopTimer = 0.0f;
	}
}