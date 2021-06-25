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
	if (transitionGO->IsActive()) {
		Debug::Log("Esta activo!");
		MoveFade();
	}
	// if (sceneUID != 0) SceneManager::ChangeScene(sceneUID);
	// if (Time::GetDeltaTime() == 0.f) Time::ResumeGame();
}

void SceneTransition::MoveFade()
{
	ComponentTransform2D* transform = transitionGO->GetComponent<ComponentTransform2D>();
	ComponentCanvas* canvas = transitionGO->GetComponent<ComponentCanvas>();
	// Size should be equal then size of the screen
	/*while (transform->GetPosition().x < 1920) {
		float actualPosition = transform->GetPosition().x;
		transform->SetPosition(float3(actualPosition * speedTransition * Time::GetDeltaTime(), 0.f, 0.f));
	}*/
	transform->SetPosition(float3(0.f, 0.f, 0.f));
}