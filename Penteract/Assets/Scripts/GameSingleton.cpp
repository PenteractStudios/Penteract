#include "GameSingleton.h"

GameSingleton* GameSingleton::GetInstance()
{
	if (!instance) {
		instance = new GameSingleton();
	}

	return instance;
}

float GameSingleton::GetVolumeScene() const {
	return volumeScene;
}

void GameSingleton::SetVolumeScene(float volumeScene_) {
	volumeScene = volumeScene_;
}