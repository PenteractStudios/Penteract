#include "SceneTransition.h"

#include "PlayerController.h"

#include "Components/UI/ComponentTransform2D.h"
#include "GameObject.h"
#include "GameplaySystems.h"
#include "CanvasFader.h"

#define FADE_SPEED 0.001f
#define CENTER_POSITION 0.f
#define ALPHA_TRANSPARENCY 255.f
#define FULL_OPACITY 1.f
#define FULL_TRANSPARENCY 0.f

EXPOSE_MEMBERS(SceneTransition) {
	MEMBER(MemberType::SCENE_RESOURCE_UID, sceneUID),
	MEMBER(MemberType::GAME_OBJECT_UID, transitionUID),
	MEMBER(MemberType::GAME_OBJECT_UID, canvasFaderObjUID),
	MEMBER(MemberType::INT, transitionMove),
	MEMBER(MemberType::FLOAT, speedTransition),
	MEMBER(MemberType::INT, levelNum)
};

GENERATE_BODY_IMPL(SceneTransition);

void SceneTransition::Start() {
	actualResolution = Screen::GetResolution();
	transitionGO = GameplaySystems::GetGameObject(transitionUID);
	if (transitionGO) {
		transform2D = transitionGO->GetComponent<ComponentTransform2D>();
		image2D = transitionGO->GetComponent<ComponentImage>();
	}

	if (canvasFaderObjUID != 0) {
		GameObject* canvasFaderObj = GameplaySystems::GetGameObject(canvasFaderObjUID);
		if (canvasFaderObj) {
			canvasFader = GET_SCRIPT(canvasFaderObj, CanvasFader);
		}
	}

}

void SceneTransition::Update() {
	if (!transform2D) return;
	if (!image2D) return;

	switch (transitionState) {
	case TransitionState::IDLE:
		UpdateObjectToResolution();
		break;
	case TransitionState::START:
		if (canvasFader) {
			if (transitionMove == static_cast<int>(TransitionMove::FADE_OUT)) {
				canvasFader->FadeOut();
			} else if (transitionMove == static_cast<int>(TransitionMove::FADE_IN)) {
				canvasFader->FadeIn();
			}
		}
		transitionState = TransitionState::IN_PROGRESS;
		break;
	case TransitionState::IN_PROGRESS: {
		bool finishedTransition = false;
		if (transitionMove == static_cast<int>(TransitionMove::LEFT_TO_RIGHT)) {
			transform2D->SetPosition(float3(transform2D->GetPosition().x + (Time::GetDeltaTime() * speedTransition), 0.f, 0.f));
			finishedTransition = transform2D->GetPosition().x == CENTER_POSITION;
		} else if (transitionMove == static_cast<int>(TransitionMove::RIGHT_TO_LEFT)) {
			transform2D->SetPosition(float3(transform2D->GetPosition().x - (Time::GetDeltaTime() * speedTransition), 0.f, 0.f));
			finishedTransition = transform2D->GetPosition().x == CENTER_POSITION;
		} else if (transitionMove == static_cast<int>(TransitionMove::TOP_TO_BOTTOM)) {
			transform2D->SetPosition(float3(0.f, transform2D->GetPosition().y - (Time::GetDeltaTime() * speedTransition), 0.f));
			finishedTransition = transform2D->GetPosition().y == CENTER_POSITION;
		} else if (transitionMove == static_cast<int>(TransitionMove::BOTTOM_TO_TOP)) {
			transform2D->SetPosition(float3(0.f, transform2D->GetPosition().y + (Time::GetDeltaTime() * speedTransition), 0.f));
			finishedTransition = transform2D->GetPosition().y == CENTER_POSITION;
		} else if (transitionMove == static_cast<int>(TransitionMove::FADE_OUT)) {
			if (!canvasFader) {
				image2D->SetColor(float4(image2D->GetColor().xyz(), Clamp(image2D->GetColor().w + (Time::GetDeltaTime() * FADE_SPEED * speedTransition), 0.0f, 1.0f)));
				finishedTransition = image2D->GetColor().w >= FULL_OPACITY;
			} else {
				finishedTransition = !canvasFader->IsPlaying();
			}
		} else if (transitionMove == static_cast<int>(TransitionMove::FADE_IN)) {
			if (!canvasFader) {
				image2D->SetColor(float4(image2D->GetColor().xyz(), Clamp(image2D->GetColor().w - (Time::GetDeltaTime() * FADE_SPEED * speedTransition), 0.0f, 1.0f)));
				finishedTransition = image2D->GetColor().w <= FULL_TRANSPARENCY;
			} else {
				finishedTransition = !canvasFader->IsPlaying();
			}
		}

		if (finishedTransition) {
			OnFinish();
		}

		break; 
	}
	case TransitionState::FINISHED:

		break;
	}


}

void SceneTransition::StartTransition(bool isExit_) {
	isExit = isExit_;
	transitionState = TransitionState::START;
}

void SceneTransition::UpdateObjectToResolution() {
	if (transitionGO) {
		float2 newResolution = Screen::GetResolution();
		if (actualResolution.x != newResolution.x || actualResolution.y != newResolution.y) {
			actualResolution = newResolution;
			transform2D->SetSize(actualResolution);
			if (transitionMove == (int)TransitionMove::LEFT_TO_RIGHT) {
				transform2D->SetPosition(float3(-actualResolution.x, 0.f, 0.f));
			} else if (transitionMove == (int)TransitionMove::RIGHT_TO_LEFT) {
				transform2D->SetPosition(float3(actualResolution.x, 0.f, 0.f));
			} else if (transitionMove == (int)TransitionMove::TOP_TO_BOTTOM) {
				transform2D->SetPosition(float3(0.f, actualResolution.y, 0.f));
			} else if (transitionMove == (int)TransitionMove::BOTTOM_TO_TOP) {
				transform2D->SetPosition(float3(0.f, -actualResolution.y, 0.f));
			}
		}
	}
}

void SceneTransition::OnFinish() {
	if (sceneUID != 0) {
		isExit ? SceneManager::ExitGame() : SceneManager::ChangeScene(sceneUID);
		if (Time::GetDeltaTime() == 0.f) Time::ResumeGame();
		if (levelNum == 3) {
			PlayerController::currentLevel = 3;
		} else if (levelNum == 2) {
			PlayerController::currentLevel = 2;
			Player::level2Upgrade = false;
		} else if (levelNum == 1) {
			PlayerController::currentLevel = 1;
			Player::level1Upgrade = false;
			Player::level2Upgrade = false;
		}
	}
	transitionState = TransitionState::FINISHED;
}
