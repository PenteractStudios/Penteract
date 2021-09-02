#include "StartButton.h"

#include "CheckpointManager.h"
#include "SceneTransition.h"
#include "PlayerController.h"

#include "GameplaySystems.h"
#include "GameObject.h"
#include "Components/UI/ComponentImage.h"

int checkpoint;

EXPOSE_MEMBERS(StartButton) {
	MEMBER(MemberType::SCENE_RESOURCE_UID, sceneUID),
	MEMBER(MemberType::GAME_OBJECT_UID, transitionUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fadeToBlackObjectUID),
	MEMBER(MemberType::INT, checkpointNum),
	MEMBER(MemberType::INT, levelNum),
	MEMBER(MemberType::FLOAT, fadeDuration)
};

GENERATE_BODY_IMPL(StartButton);

void StartButton::Start() {
	/* Audio */
	selectable = GetOwner().GetComponent<ComponentSelectable>();

	int i = 0;
	for (ComponentAudioSource& src : GetOwner().GetComponents<ComponentAudioSource>()) {
		if (i < static_cast<int>(UIAudio::TOTAL)) audios[i] = &src;
		++i;
	}

	if (transitionUID != 0) {
		transitionGO = GameplaySystems::GetGameObject(transitionUID);
		if (transitionGO) sceneTransition = GET_SCRIPT(transitionGO, SceneTransition);
	}

	GameObject* fadeToBlackObject = GameplaySystems::GetGameObject(fadeToBlackObjectUID);

	if (fadeToBlackObject) {
		fadeToBlackImage = fadeToBlackObject->GetComponent<ComponentImage>();
	}

}

void StartButton::Update() {
	/* Audio */

	if (pressed && fadeToBlackImage) {
		fadeTimer += Time::GetDeltaTime();
		float delta = Clamp01(fadeTimer / fadeDuration);

		fadeToBlackImage->SetColor(float4(0, 0, 0, delta));

		if (fadeTimer >= fadeDuration) {
			DoTransition();
		}

	} else {
		if (selectable) {
			ComponentEventSystem* eventSystem = UserInterface::GetCurrentEventSystem();
			if (eventSystem) {
				ComponentSelectable* hoveredComponent = eventSystem->GetCurrentlyHovered();
				if (hoveredComponent) {
					bool hovered = selectable->GetID() == hoveredComponent->GetID() ? true : false;
					if (hovered) {
						if (playHoveredAudio) {
							PlayAudio(UIAudio::HOVERED);
							playHoveredAudio = false;
						}
					} else {
						playHoveredAudio = true;
					}
				} else {
					playHoveredAudio = true;
				}
			}
		}
	}
}

void StartButton::OnButtonClick() {

	PlayAudio(UIAudio::CLICKED);

	if (!fadeToBlackImage) {
		DoTransition();
	} else {
		pressed = true;
		fadeTimer = 0.0f;
	}

}

void StartButton::DoTransition() {
	checkpoint = checkpointNum;

	if (sceneTransition) {
		sceneTransition->StartTransition();
	} else {
		if (sceneUID != 0) SceneManager::ChangeScene(sceneUID);
		if (levelNum == 2) {
			PlayerController::currentLevel = 2;
			Player::level2Upgrade = false;
		} else if (levelNum == 1) {
			PlayerController::currentLevel = 1;
			Player::level1Upgrade = false;
			Player::level2Upgrade = false;
		}
		if (Time::GetDeltaTime() == 0.f) Time::ResumeGame();
	}
}

void StartButton::PlayAudio(UIAudio type) {
	if (audios[static_cast<int>(type)]) audios[static_cast<int>(type)]->Play();
}