#pragma once

#include "Scripting/Script.h"

#include "TilesPatterns.h"

#include <vector>

class ComponentAudioSource;
class ComponentTransform;

class FloorIsLava : public Script
{
	GENERATE_BODY(FloorIsLava);

public:

	enum class Audios {
		OPEN,
		CLOSE,
		TOTAL
	};

	void Start() override;
	void Update() override;

	void StartFire();
	void StopFire();
	void SetInterphase(bool interphaseActive);

public:
	std::string container = "";

	float timeTilesActiveNormal = 0.5f;
	float timeTilesActiveInterphase = 2.f;
	float timeWarning = 1.5f;

	bool sequential = false;

	UID playerUID = 0;
	float soundDistance = 10;
	UID fireAudioUID = 0;

private:
	std::vector<GameObject*> tiles;

	int nextPattern = 1;
	int currentPattern = 1;

	const bool* currentTilesPattern = nullptr;
	const bool* nextTilesPattern = nullptr;

	const TilesPattern* sequentialCorridorPatterns = nullptr;
	const TilesPattern* sequentialBossPatternsNormal = nullptr;
	const TilesPattern* sequentialBossPatternsInterphase = nullptr;

	const bool* pattern1 = nullptr;
	const bool* pattern2 = nullptr;
	const bool* pattern3 = nullptr;

	float timeTilesActive = 0.f;
	float timeRemainingTilesActive = 0.f;
	float timeRemainingWarning = 0.f;

	float timerTilesClosing = 2.f;
	float timerTilesClosingRemaining = 0.f;

	bool warningActive = true;
	bool fireActive = false;
	bool patternFinished = true;

	bool firstTimeWarning = true;
	bool firstTimeNextWarning = true;
	bool firstTimeFireActive = true;
	bool firsTimeFireStopped = true;

	int sequentialCount = 0;

	bool started = false;

	bool interphase = false;

	std::vector<ComponentAudioSource*> audiosFireTilesOpening;
	std::vector<ComponentAudioSource*> audiosFireTilesClosing;

	GameObject* playerGameObject = nullptr;
	float3 playerPosition = float3(0.0f, 0.0f, 0.0f);
	GameObject* fireAudioGameObject = nullptr;

private:
	void SetRandomPattern(int pattern, const bool*& boolPattern);
	void SetSequentialPatterns(int countSize, const TilesPattern* sequentialPattern);
	void UpdateWarningTiles();
	void UpdateWarningNextTiles();
	void UpdateFireActiveTiles(bool activate);	
};

