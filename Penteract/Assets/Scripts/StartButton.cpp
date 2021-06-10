#include "StartButton.h"

#include "CheckpointManager.h"
#include "SceneTransition.h"

#include "GameplaySystems.h"
#include "GameObject.h"

int checkpoint;

EXPOSE_MEMBERS(StartButton) {
	MEMBER(MemberType::SCENE_RESOURCE_UID, sceneUID),
	MEMBER(MemberType::GAME_OBJECT_UID, transitionUID),
	MEMBER(MemberType::INT, checkpointNum),
};

GENERATE_BODY_IMPL(StartButton);

void StartButton::Start() {
	if (transitionUID != 0) {
		transitionGO = GameplaySystems::GetGameObject(transitionUID);
		if (transitionGO) sceneTransition = GET_SCRIPT(transitionGO, SceneTransition);
	}
}

void StartButton::Update() {

}

void StartButton::OnButtonClick() {
	checkpoint = checkpointNum;
	if (sceneTransition) {
		sceneTransition->InitTransition();
		while (sceneTransition->initTransition) {
			// Cutre-concurrencia
			Debug::Log("Se esta ejecutando la transicion");
		}
	}
	//if (sceneUID != 0) SceneManager::ChangeScene(sceneUID);
	//if (Time::GetDeltaTime() == 0.f) Time::ResumeGame();
	
}