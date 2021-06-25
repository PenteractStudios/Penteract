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
	/*
	Debug::Log("Pulsado boton pero funciona Update");
	if (sceneTransition) {
		if (sceneTransition->finishedTransition) {
			if (sceneUID != 0) SceneManager::ChangeScene(sceneUID);
			if (Time::GetDeltaTime() == 0.f) Time::ResumeGame();
		}
	}*/
}

void StartButton::OnButtonClick() {
	checkpoint = checkpointNum;
	if (sceneTransition) {
		transitionGO->Enable();
		sceneTransition->Update();
	} else {
		if (sceneUID != 0) SceneManager::ChangeScene(sceneUID);
		if (Time::GetDeltaTime() == 0.f) Time::ResumeGame();
	}
}