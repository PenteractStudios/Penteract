#pragma once

#include "Scripting/Script.h"

#include "TilesPatterns.h"

#include <vector>

class FloorIsLava : public Script
{
	GENERATE_BODY(FloorIsLava);

public:

	void Start() override;
	void Update() override;

	void StartFire();
	void StopFire();

public:
	std::string container = "";

	float timeTilesActive = 10.f;
	float timeWarning = 3.f;

	bool sequential = false;

private:
	std::vector<GameObject*> tiles;

	int nextPattern = 1;
	int currentPattern = 1;

	const bool* currentTilesPattern = nullptr;
	const bool* nextTilesPattern = nullptr;

	const TilesPattern* sequentialPatterns = nullptr;

	const bool* pattern1 = nullptr;
	const bool* pattern2 = nullptr;
	const bool* pattern3 = nullptr;

	float timeRemainingTilesActive = 0.f;
	float timeRemainingWarning = 0.f;

	float timerTilesClosing = 2.f;
	float timerTilesClosingRemaining = 0.f;

	bool warningActive = true;
	bool fireActive = false;
	bool patternFinished = true;

	bool firstTimeWarning = true;
	bool firstTimeFireActive = true;

	int sequentialCount = 0;

	bool started = false;

private:
	void SetPattern(int pattern, const bool*& boolPattern);
	void UpdateWarningTiles(bool activate);
	void UpdateFireActiveTiles(bool activate);
};

