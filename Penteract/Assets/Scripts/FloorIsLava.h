#pragma once

#include "Scripting/Script.h"

#define CORRIDOR_TILES 28
#define ARENA_TILES 32

class FloorIsLava : public Script
{
	GENERATE_BODY(FloorIsLava);

public:

	void Start() override;
	void Update() override;

public:
	UID corridorUID = 0;
	UID arenaUID = 0;

	float timeTilesActive = 10.f;
	float timeWarning = 3.f;

private:

	GameObject* corridor = nullptr;
	GameObject* arena = nullptr;

	GameObject* corridorTiles[CORRIDOR_TILES];
	GameObject* arenaTiles[ARENA_TILES];

	int previousPattern = 1;
	int currentPattern = 1;

	bool* currentCorridorPattern;
	bool corridorPattern1[CORRIDOR_TILES] = {	true, false, true, false, true, false, false, false, false, true,
												false, true, false, true, false, false, true, false, true, false, 
												true, false, false, false, false, true, false, true};
	bool corridorPattern2[CORRIDOR_TILES] = {	false, false, false, false, true, true, true, true, false, false,
												false, false, true, true, true, true, false, false, false, false,
												true, true, true, true, false, false, false, false};
	bool corridorPattern3[CORRIDOR_TILES] = {	true, true, true, true, false, false, false, false, true, true,
												true, true, false, false, false, false, true, true, true, true,
												false, false, false, false, true, true, true, true};

	bool* currentArenaPattern;
	bool arenaPattern1[ARENA_TILES] = { true, true, true, true, true, true, true, true, true, true,
										true, true, true, true, true, true, true, true, true, true,
										true, true, true, true, false, false, false, false, false, false, 
										false, false};
	bool arenaPattern2[ARENA_TILES] = { false, false, false, false, false, false, false, false, false, false, 
										false, false, false, false, false, false, false, false, false, false, 
										false, false, false, false, true, true, true, true, true, true, 
										true, true};
	bool arenaPattern3[ARENA_TILES] = { true, true, true, false, true, true, true, true, true, false,
										true, true, true, true, true, false, true, true, true, true, 
										true, false, true, true, true, false, true, false, true, false,
										true, false};

	float timeRemainingTilesActive = 0.f;
	float timeRemainingWarning = 0.f;

	float timerTilesClosing = 2.f;
	float timerTilesClosingRemaining = 0.f;

	bool warningActive = true;
	bool fireActive = false;
	bool patternFinished = true;

	bool firstTimeWarning = true;
	bool firstTimeFireActive = true;

private:
	void UpdateWarningTiles(bool activate);
	void UpdateFireActiveTiles(bool activate);
};

