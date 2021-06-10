#include "SceneTransition.h"

#include "Components/UI/ComponentTransform2D.h"
#include "GameObject.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(SceneTransition) {
	MEMBER(MemberType::GAME_OBJECT_UID, transitionUID),
	MEMBER(MemberType::INT, speedTransition),
	MEMBER(MemberType::BOOL, initTransition),
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
			/*ComponentTransform2D* transform = transitionGO->GetComponent<ComponentTransform2D>();
			while (transform->GetPosition().x < transform->GetSize().x) {
				float actualPosition = transform->GetPosition().x;
				transform->SetPosition(float3(actualPosition * speedTransition * Time::GetDeltaTime(), 0.f, 0.f));
			}*/
			StopTransition();
		}
	} else {
		Debug::Log("No inicia la transicion");
	}
}

void SceneTransition::InitTransition() {
	transitionGO->Enable();
	initTransition = true;
}

void SceneTransition::StopTransition()
{
	transitionGO->Disable();
	initTransition = false;
}