#include "SceneTransition.h"

#include "Components/UI/ComponentTransform2D.h"
#include "GameObject.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(SceneTransition) {
	MEMBER(MemberType::GAME_OBJECT_UID, transitionUID),
	MEMBER(MemberType::SCENE_RESOURCE_UID, sceneUID),
	MEMBER(MemberType::INT, speedTransition)
};

GENERATE_BODY_IMPL(SceneTransition);

void SceneTransition::Start() {
	transitionGO = GameplaySystems::GetGameObject(transitionUID);
	if (transitionGO) {
		transform2D = transitionGO->GetComponent<ComponentTransform2D>();
	}
}

void SceneTransition::Update() {
	if (startTransition) {
		if (finishedTransition) {
			if (sceneUID != 0) SceneManager::ChangeScene(sceneUID);
			if (Time::GetDeltaTime() == 0.f) Time::ResumeGame();
		}
		else {
			// Transition
			// Size should be equal then size of the screen
			transform2D->SetPosition(float3(transform2D->GetPosition().x + (1.f * speedTransition), 0.f, 0.f));
			// transform2D->SetPosition(float3(0.f, 0.f, 0.f));
			finishedTransition = transform2D->GetPosition().x == 0.f;
		}
	}
}

void SceneTransition::StartTransition()
{
	startTransition = true;
}
