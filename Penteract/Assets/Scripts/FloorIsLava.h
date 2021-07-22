#pragma once

#include "Scripting/Script.h"

#define CORRIDOR_ROWS 2
#define CORRIDOR_COLS 18
#define ARENA_ROWS 8
#define ARENA_COLS 5

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

	GameObject* corridorTiles[CORRIDOR_ROWS][CORRIDOR_COLS];
	GameObject* arenaTiles[ARENA_ROWS][ARENA_COLS];

	int previousPattern = 1;
	int currentPattern = 1;

	bool currentCorridorPattern[CORRIDOR_ROWS][CORRIDOR_COLS];
	bool corridorPattern1[CORRIDOR_ROWS][CORRIDOR_COLS];
	bool corridorPattern2[CORRIDOR_ROWS][CORRIDOR_COLS];
	bool corridorPattern3[CORRIDOR_ROWS][CORRIDOR_COLS];

	bool currentArenaPattern[ARENA_ROWS][ARENA_COLS];
	bool arenaPattern1[ARENA_ROWS][ARENA_COLS];
	bool arenaPattern2[ARENA_ROWS][ARENA_COLS];
	bool arenaPattern3[ARENA_ROWS][ARENA_COLS];

	float timeRemainingTilesActive = 0.f;
	float timeRemainingWarning = 0.f;

	bool warningActive = true;
	bool fireActive = false;
	bool patternFinished = true;

	bool firstTimeWarning = true;
	bool firstTimeFireActive = true;

private:
	void UpdateWarningMatrices(bool activate);
	void UpdateFireActiveMatrices(bool activate);

};

