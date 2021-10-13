#include "AudioController.h"

#include "GameplaySystems.h"

EXPOSE_MEMBERS(AudioController) {
    // Add members here to expose them to the engine. Example:
	MEMBER(MemberType::FLOAT, mainGain),
	MEMBER(MemberType::FLOAT, musicGain),
	MEMBER(MemberType::FLOAT, sfxGain)
};

GENERATE_BODY_IMPL(AudioController);

void AudioController::Start() {
	Audio::SetGainMainChannel(GameplaySystems::GetGlobalVariable("mainGain", mainGain));
	Audio::SetGainMusicChannel(GameplaySystems::GetGlobalVariable("musicGain", musicGain));
	Audio::SetGainSFXChannel(GameplaySystems::GetGlobalVariable("sfxGain", sfxGain));
}

void AudioController::Update() {
	GameplaySystems::SetGlobalVariable("mainGain", Audio::GetGainMainChannel());
	GameplaySystems::SetGlobalVariable("musicGain", Audio::GetGainMusicChannel());
	GameplaySystems::SetGlobalVariable("sfxGain", Audio::GetGainSFXChannel());
	
}