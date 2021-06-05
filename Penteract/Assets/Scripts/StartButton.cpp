#include "StartButton.h"

#include "CheckpointManager.h"

#include "GameplaySystems.h"
#include "GameObject.h"

int checkpoint;

EXPOSE_MEMBERS(StartButton) {
	MEMBER(MemberType::INT, checkpointNum),
};


GENERATE_BODY_IMPL(StartButton);

void StartButton::Start() {
}

void StartButton::Update() {
}

void StartButton::OnButtonClick() {
	checkpoint = checkpointNum;
	//SceneManager::ChangeScene("Assets/Scenes/Level1.scene");
	SceneManager::ChangeScene("Assets/Scenes/WinConTest.scene");
	if (Time::GetDeltaTime() == 0.f) Time::ResumeGame();
}