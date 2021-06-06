#include "VolumeControl.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "Components/ComponentAudioListener.h"
#include "Components/UI/ComponentSlider.h"

#include "GameManager.h"

EXPOSE_MEMBERS(VolumeControl) {
    // Add members here to expose them to the engine. Example:
	MEMBER(MemberType::GAME_OBJECT_UID, sliderUID),
	MEMBER(MemberType::GAME_OBJECT_UID, gameCameraUID),
	MEMBER(MemberType::GAME_OBJECT_UID, gameManagerUID),
};

GENERATE_BODY_IMPL(VolumeControl);

void VolumeControl::Start() {
	sliderGO = GameplaySystems::GetGameObject(sliderUID);
	gameCamera = GameplaySystems::GetGameObject(gameCameraUID);
	gameManager = GameplaySystems::GetGameObject(gameManagerUID);
	if (sliderGO && gameCamera) {
		ComponentSlider* comSlider = sliderGO->GetComponent<ComponentSlider>();
		ComponentAudioListener* comAudioLis = gameCamera->GetComponent<ComponentAudioListener>();
		if (comSlider && comAudioLis) {
			float volume = 0.f; 

			// To control volume between scenes in GameManager
			if (gameManager) {
				gameManagerController = GET_SCRIPT(gameManager, GameManager);
				volume = gameManagerController->volumeScene;
				// Update the slider
				// comSlider->SetNormalizedValue(volume);
			}
			else {
				volume = comSlider->GetNormalizedValue();
			}

			comAudioLis->SetAudioVolume(volume);
		}
	}
}

void VolumeControl::Update() {
	
}

void VolumeControl::OnValueChanged() {
	if (!sliderGO || !gameCamera) return;
	ComponentSlider* comSlider = sliderGO->GetComponent<ComponentSlider>();
	ComponentAudioListener* comAudioLis = gameCamera->GetComponent<ComponentAudioListener>();
	if (!comSlider || !comAudioLis) return;
	
	float volume = comSlider->GetNormalizedValue();
	
	// To control volume between scenes in GameManager
	if (gameManager) {
		gameManagerController = GET_SCRIPT(gameManager, GameManager);
		gameManagerController->volumeScene = volume;
	}

	comAudioLis->SetAudioVolume(volume);
}