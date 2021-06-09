#pragma once

#ifdef BUILD_DLL
	#define DLL_EXPORT __declspec(dllexport)
#else
	#define DLL_EXPORT __declspec(dllimport)
#endif

class GameSingleton
{

public:
	GameSingleton(const GameSingleton*) = delete;

	GameSingleton() {}; // Should be private but fails in GENERATE_BODY(GameManager);

	DLL_EXPORT static GameSingleton* GetInstance();

	// --------- GETTERS ---------- //
	float GetVolumeScene() const;

	// --------- SETTERS ---------- //
	void SetVolumeScene(float volumeScene_);

public:
	// Here we put all the global variables of the game as audio, points, etc...
	float volumeScene = 0.5f;

private:
	inline static GameSingleton* instance = nullptr;
};
