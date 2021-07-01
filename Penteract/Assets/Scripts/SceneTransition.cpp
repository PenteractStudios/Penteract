#include "SceneTransition.h"

#include "Components/UI/ComponentTransform2D.h"
#include "GameObject.h"
#include "GameplaySystems.h"

#define FADE_SPEED 0.01f
#define CENTER_POSITION 0.f
#define ALPHA_TRANSPARENCY 255.f
#define FULL_TRANSPARENCY 1.f

EXPOSE_MEMBERS(SceneTransition) {
	MEMBER(MemberType::SCENE_RESOURCE_UID, sceneUID),
	MEMBER(MemberType::GAME_OBJECT_UID, transitionUID),
	MEMBER(MemberType::INT, transitionMove),
	MEMBER(MemberType::FLOAT, speedTransition),
};

GENERATE_BODY_IMPL(SceneTransition);

void SceneTransition::Start() {
	actualResolution = Screen::GetResolution();
	transitionGO = GameplaySystems::GetGameObject(transitionUID);
	if (transitionGO) {
		transform2D = transitionGO->GetComponent<ComponentTransform2D>();
		image2D = transitionGO->GetComponent<ComponentImage>();
	}
}

void SceneTransition::Update() {
	if (startTransition) {
		if (finishedTransition) {
			if (sceneUID != 0) {
				isExit ? SceneManager::ExitGame() : SceneManager::ChangeScene(sceneUID);
				if (Time::GetDeltaTime() == 0.f) Time::ResumeGame();
			}
		}
		else {
			// Transition
			// Size should be equal then size of the screen
			if (transitionMove == (int)TransitionMove::LEFT_TO_RIGHT) {
				transform2D->SetPosition(float3(transform2D->GetPosition().x + (1.f * speedTransition), 0.f, 0.f));
				finishedTransition = transform2D->GetPosition().x == CENTER_POSITION;
			} else if (transitionMove == (int)TransitionMove::RIGHT_TO_LEFT) {
				transform2D->SetPosition(float3(transform2D->GetPosition().x - (1.f * speedTransition), 0.f, 0.f));
				finishedTransition = transform2D->GetPosition().x == CENTER_POSITION;
			} else if (transitionMove == (int)TransitionMove::TOP_TO_BOTTOM) {
				transform2D->SetPosition(float3(0.f, transform2D->GetPosition().y - (1.f * speedTransition), 0.f));
				finishedTransition = transform2D->GetPosition().y == CENTER_POSITION;
			} else if (transitionMove == (int)TransitionMove::BOTTOM_TO_TOP) {
				transform2D->SetPosition(float3(0.f, transform2D->GetPosition().y + (1.f * speedTransition), 0.f));
				finishedTransition = transform2D->GetPosition().y == CENTER_POSITION;
			} else if (transitionMove == (int)TransitionMove::NONE) {
				if (image2D) {
					image2D->SetColor(float4(image2D->GetColor().xyz(), image2D->GetColor().w + (FADE_SPEED * speedTransition)));
					finishedTransition = image2D->GetColor().w >= FULL_TRANSPARENCY;
				} else {
					finishedTransition = true;
				}
			}
		}
	} else {
		UpdateObjectToResolution();
	}
}

void SceneTransition::StartTransition(bool isExit_)
{
	isExit = isExit_;
	startTransition = true;
}

void SceneTransition::UpdateObjectToResolution()
{
	if (transitionGO) {
		float2 newResolution = Screen::GetResolution();
		if (actualResolution.x != newResolution.x || actualResolution.y != newResolution.y) {
			actualResolution = newResolution;
			transform2D->SetSize(actualResolution);
			if (transitionMove == (int)TransitionMove::LEFT_TO_RIGHT) {
				transform2D->SetPosition(float3(-actualResolution.x, 0.f, 0.f));
			}
			else if (transitionMove == (int)TransitionMove::RIGHT_TO_LEFT) {
				transform2D->SetPosition(float3(actualResolution.x, 0.f, 0.f));
			}
			else if (transitionMove == (int)TransitionMove::TOP_TO_BOTTOM) {
				transform2D->SetPosition(float3(0.f, actualResolution.y, 0.f));
			}
			else if (transitionMove == (int)TransitionMove::BOTTOM_TO_TOP) {
				transform2D->SetPosition(float3(0.f, -actualResolution.y, 0.f));
			}
		}
	}
}
