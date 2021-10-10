#include "GameOverUIController.h"

#include "UISpriteSheetPlayer.h"
#include "GameplaySystems.h"
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentText.h"
#include "Components/ComponentAudioSource.h"
#include "CanvasFader.h"

//TODO MAKE BUTTONS DO WHAT THEY SAY THEY DO
//TODO MAKE BACKGROUND ANIMATED
//TODO ADJUST UI SIZES

#define HIERARCHY_INDEX_PLAY_AGAIN_TEXT 2

EXPOSE_MEMBERS(GameOverUIController) {
	MEMBER(MemberType::GAME_OBJECT_UID, inOutPlayerUID),
	MEMBER(MemberType::GAME_OBJECT_UID, loopPlayerUID),
	MEMBER(MemberType::GAME_OBJECT_UID, vibrationPlayerUID),
	MEMBER(MemberType::GAME_OBJECT_UID, outInPlayerUID),
	MEMBER(MemberType::FLOAT, minVibrationTime),
	MEMBER(MemberType::FLOAT, maxVibrationTime),
	MEMBER(MemberType::FLOAT, fadeInTime),
	MEMBER(MemberType::GAME_OBJECT_UID, playAgainButtonUID),
	MEMBER(MemberType::GAME_OBJECT_UID, mainMenuButtonUID),
	MEMBER(MemberType::GAME_OBJECT_UID, exitButtonUID),
	MEMBER(MemberType::GAME_OBJECT_UID, backgroundUID),
	MEMBER(MemberType::GAME_OBJECT_UID, scrollingBackgroundObjUID),
	MEMBER(MemberType::GAME_OBJECT_UID, canvasFaderObjUID),
	MEMBER(MemberType::GAME_OBJECT_UID, canvasPlayerHUDObjUID),
	MEMBER(MemberType::FLOAT, scrollDuration)
};

GENERATE_BODY_IMPL(GameOverUIController);

void GameOverUIController::Start() {

	GameObject* inOutPlayerGO = GameplaySystems::GetGameObject(inOutPlayerUID);
	GameObject* loopPlayerGO = GameplaySystems::GetGameObject(loopPlayerUID);
	GameObject* vibrationPlayerGO = GameplaySystems::GetGameObject(vibrationPlayerUID);
	GameObject* outInPlayerGO = GameplaySystems::GetGameObject(outInPlayerUID);

	if (inOutPlayerGO) inOutPlayer = GET_SCRIPT(inOutPlayerGO, UISpriteSheetPlayer);
	if (loopPlayerGO) loopPlayer = GET_SCRIPT(loopPlayerGO, UISpriteSheetPlayer);
	if (vibrationPlayerGO) vibrationPlayer = GET_SCRIPT(vibrationPlayerGO, UISpriteSheetPlayer);
	if (outInPlayerGO) outInPlayer = GET_SCRIPT(outInPlayerGO, UISpriteSheetPlayer);

	GameObject* playAgainGO = GameplaySystems::GetGameObject(playAgainButtonUID);
	GameObject* mainMenuGO = GameplaySystems::GetGameObject(mainMenuButtonUID);
	GameObject* exitButtonGO = GameplaySystems::GetGameObject(exitButtonUID);
	GameObject* backgroundGO = GameplaySystems::GetGameObject(backgroundUID);
	GameObject* scrollingBackgroundObj = GameplaySystems::GetGameObject(scrollingBackgroundObjUID);

	canvasPlayerHUDObj = GameplaySystems::GetGameObject(canvasPlayerHUDObjUID);

	if (canvasFaderObjUID != 0) {
		GameObject* canvasFaderObj = GameplaySystems::GetGameObject(canvasFaderObjUID);
		if (canvasFaderObj) {
			canvasFader = GET_SCRIPT(canvasFaderObj, CanvasFader);
		}
	}


	if (playAgainGO) {
		playAgainButtonImage = playAgainGO->GetComponent<ComponentImage>();
		std::vector<GameObject*>children = playAgainGO->GetChildren();
		if (children.size() > HIERARCHY_INDEX_PLAY_AGAIN_TEXT - 1) {
			playAgainButtonText = children[HIERARCHY_INDEX_PLAY_AGAIN_TEXT]->GetComponent < ComponentText>();
		}
	}
	if (mainMenuGO) {
		mainMenuButtonImage = mainMenuGO->GetComponent<ComponentImage>();

		std::vector<GameObject*>children = mainMenuGO->GetChildren();
		if (children.size() > HIERARCHY_INDEX_PLAY_AGAIN_TEXT - 1) {
			mainMenuButtonText = children[HIERARCHY_INDEX_PLAY_AGAIN_TEXT]->GetComponent < ComponentText>();
		}
	}
	if (exitButtonGO) {
		exitButtonImage = exitButtonGO->GetComponent<ComponentImage>();

		std::vector<GameObject*>children = exitButtonGO->GetChildren();
		if (children.size() > HIERARCHY_INDEX_PLAY_AGAIN_TEXT - 1) {
			exitButtonText = children[HIERARCHY_INDEX_PLAY_AGAIN_TEXT]->GetComponent < ComponentText>();
		}
	}

	if (backgroundGO) {
		backgroundImage = backgroundGO->GetComponent<ComponentImage>();
	}

	if (scrollingBackgroundObj) {
		scrollingBackgroundImage = scrollingBackgroundObj->GetComponent<ComponentImage>();
	}



	std::vector<GameObject*> children = GetOwner().GetChildren();

	for (std::vector<GameObject*>::const_iterator it = children.begin(); it != children.end(); ++it) {
		(*it)->Disable();
	}

}

void GameOverUIController::Update() {
	if (!inOutPlayer || !loopPlayer || !vibrationPlayer || !outInPlayer) return;

	if (state != GameOverState::FADE_OUT && state != GameOverState::OFFLINE) {
		if (canvasFader) {
			if (canvasFader->IsPlaying()) {
				Debug::Log("CanvasFaderPlaying");
				if (outInPlayer) {
					state = GameOverState::FADE_OUT;
					if (inOutPlayer)inOutPlayer->Stop();
					if (loopPlayer)loopPlayer->Stop();
					if (vibrationPlayer)vibrationPlayer->Stop();
					outInPlayer->Play();
				}
			}
		}
	}

	if (scrollingBackgroundImage) {
		if (scrollingTimer < scrollDuration) {
			scrollingTimer += Time::GetDeltaTime();
			scrollingBackgroundImage->SetTextureOffset(float2(-scrollingTimer * 0.1f, scrollingTimer * 0.1f));
		} else {
			scrollingTimer = 0.0f;
		}
	}
	switch (state) {
	case GameOverState::OFFLINE:
		break;
	case GameOverState::FADE_IN: {
		fadeInTimer += Time::GetDeltaTime();
		float delta = Clamp01(fadeInTimer / fadeInTime);
		SetColors(delta);

		if (delta >= 1.0f) {
			state = GameOverState::START;
		}
		if (delta >= 0.5f && !inOutPlayer->IsPlaying()) {
			inOutPlayer->Play();
		}

		break;
	}
	case GameOverState::START:
		if (!inOutPlayer->IsPlaying()) {
			EnterIdleState();
		}
		break;
	case GameOverState::IDLE:

		vibrationTimer -= Time::GetDeltaTime();

		if (vibrationTimer <= 0) {
			state = GameOverState::VIBRATION;
			vibrationTimer = 0.0f;
			loopPlayer->Stop();
			vibrationPlayer->Play();
		}

		break;
	case GameOverState::VIBRATION:
		if (!vibrationPlayer->IsPlaying()) {
			EnterIdleState();
		}
		break;
	case GameOverState::FADE_OUT:
		break;
	}
}

void GameOverUIController::GameOver() {
	GetOwner().Enable();
	DisablePlayerHUD();
	Screen::SetChromaticAberration(false);
	if (inOutPlayer && state == GameOverState::OFFLINE) {
		fadeInTimer = 0.0f;
		state = GameOverState::FADE_IN;

		std::vector<GameObject*> children = GetOwner().GetChildren();

		for (std::vector<GameObject*>::const_iterator it = children.begin(); it != children.end(); ++it) {
			(*it)->Enable();
		}

		if (backgroundImage && playAgainButtonImage && mainMenuButtonImage && scrollingBackgroundImage) {
			backgroundOriginalColor = backgroundImage->GetColor();
			playAgainButtonImageOriginalColor = playAgainButtonImage->GetColor();
			mainMenuButtonImageOriginalColor = mainMenuButtonImage->GetColor();
			scrollingBackgroundImageOriginalColor = scrollingBackgroundImage->GetColor();
			SetColors(0);
		}
	}
	ComponentAudioSource* audio = GetOwner().GetParent()->GetComponent<ComponentAudioSource>();
	if (audio && audio->IsPlaying()) {
		audio->Stop();
	}
}

void GameOverUIController::EnterIdleState() {
	state = GameOverState::IDLE;
	loopPlayer->Play();
	vibrationTimer = minVibrationTime + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxVibrationTime - minVibrationTime)));
}

void GameOverUIController::SetColors(float delta) {
	if (!playAgainButtonImage || !playAgainButtonText || !mainMenuButtonImage || !mainMenuButtonText || !backgroundImage)return;

	playAgainButtonImage->SetColor(float4(playAgainButtonImageOriginalColor.xyz(), delta));
	playAgainButtonText->SetFontColor(float4(1, 1, 1, delta));
	mainMenuButtonImage->SetColor(float4(mainMenuButtonImageOriginalColor.xyz(), delta));
	mainMenuButtonText->SetFontColor(float4(1, 1, 1, delta));
	backgroundImage->SetColor(float4(backgroundOriginalColor.xyz(), delta * backgroundOriginalColor.w));

	scrollingBackgroundImage->SetColor(float4(scrollingBackgroundImageOriginalColor.xyz(), delta * scrollingBackgroundImageOriginalColor.w));
}

void GameOverUIController::DisablePlayerHUD() {
	if (canvasPlayerHUDObj) {
		canvasPlayerHUDObj->Disable();
	}
}
