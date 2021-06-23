#include "SceneTransition.h"

#include "Components/UI/ComponentTransform2D.h"
#include "GameObject.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(SceneTransition) {
	MEMBER(MemberType::GAME_OBJECT_UID, transitionUID),
	MEMBER(MemberType::INT, speedTransition),
	MEMBER(MemberType::BOOL, initTransition),
	MEMBER(MemberType::BOOL, finishedTransition),
};

GENERATE_BODY_IMPL(SceneTransition);

void SceneTransition::Start() {
	transitionGO = GameplaySystems::GetGameObject(transitionUID);
	if (transitionGO) {
		transform2D = transitionGO->GetComponent<ComponentTransform2D>();
	}
}

void SceneTransition::Update() {
	if (initTransition) {
		if (transitionGO) {
			MoveFade();
			StopTransition();
		}
	} else {
		Debug::Log("Dont start the transition");
	}
}

void SceneTransition::MoveFade()
{
	ComponentTransform2D* transform = transitionGO->GetComponent<ComponentTransform2D>();
	ComponentCanvas* canvas = transitionGO->GetComponent<ComponentCanvas>();
	// Size should be equal then size of the screen
	while (transform->GetPosition().x < 1920) {
		float actualPosition = transform->GetPosition().x;
		transform->SetPosition(float3(actualPosition * speedTransition * Time::GetDeltaTime(), 0.f, 0.f));
	}
}

void SceneTransition::InitTransition() {
	initTransition = true;
	transitionGO->Enable();
	Update();
}

void SceneTransition::StopTransition()
{
	initTransition = false;
	transitionGO->Disable();
	finishedTransition = true;
}