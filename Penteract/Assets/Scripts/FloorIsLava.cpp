#include "FloorIsLava.h"

#include "GameObject.h"
#include "GameplaySystems.h"

#include <math.h>

EXPOSE_MEMBERS(FloorIsLava) {
    MEMBER(MemberType::GAME_OBJECT_UID, corridorUID),
    MEMBER(MemberType::GAME_OBJECT_UID, arenaUID),
	MEMBER(MemberType::FLOAT, timeTilesActive),
	MEMBER(MemberType::FLOAT, timeWarning)

};

GENERATE_BODY_IMPL(FloorIsLava);

void FloorIsLava::Start() {
	
	corridor = GameplaySystems::GetGameObject(corridorUID);
	if (corridor) {
		int i, j = 0;
		for (GameObject* children : corridor->GetChildren()) {
			if (j >= CORRIDOR_COLS) {
				j = 0;
				if (i < CORRIDOR_ROWS) ++i;
			}
			corridorTiles[i][j] = children;
			++j;
		}
	}
	arena = GameplaySystems::GetGameObject(arenaUID);
	if (arena) {
		int i, j = 0;
		for (GameObject* children : arena->GetChildren()) {
			if (j >= ARENA_COLS) {
				j = 0;
				if (i < ARENA_COLS) ++i;
			}
			arenaTiles[i][j] = children;
			++j;
		}
	}

	timeRemainingWarning = timeWarning;
	currentPattern = previousPattern;
}

void FloorIsLava::Update() {
	
	//select a random corridor and arena pattern
	if (patternFinished) {
		while (currentPattern == previousPattern) {
			currentPattern = 1 + (rand() % 3);
		}
		switch (currentPattern)
		{
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		default:
			break;
		}
	}
	
	if (warningActive) {
		//corridor
		for (int i = 0; i < CORRIDOR_ROWS; ++i) {
			for (int j = 0; j < CORRIDOR_COLS; ++j) {
				if (currentCorridorPattern[i][j]) {
					if (corridorTiles[i][j]) {
						//corridorTiles[i][j].activate warning particles
					}					
				}
			}
		}

		//arena
		for (int i = 0; i < ARENA_ROWS; ++i) {
			for (int j = 0; j < ARENA_COLS; ++j) {
				if (currentArenaPattern[i][j]) {
					if (arenaTiles[i][j]) {
						//arenaTiles[i][j].activate warning particles
					}
				}
			}
		}

		if (timeRemainingWarning > 0.f) {
			timeRemainingWarning -= Time::GetDeltaTime();
		}
		else {
			warningActive = false;
			fireActive = true;
			timeRemainingTilesActive = timeTilesActive;
		}
	}

	if (fireActive) {
		//corridor
		for (int i = 0; i < CORRIDOR_ROWS; ++i) {
			for (int j = 0; j < CORRIDOR_COLS; ++j) {
				if (currentCorridorPattern[i][j]) {
					if (corridorTiles[i][j]) {
						ComponentBoxCollider* boxCollider = corridorTiles[i][j]->GetComponent<ComponentBoxCollider>();
						if (boxCollider && !boxCollider->IsActive()) {
							boxCollider->Enable();
						}
					}
				}
			}
		}

		//arena
		for (int i = 0; i < ARENA_ROWS; ++i) {
			for (int j = 0; j < ARENA_COLS; ++j) {
				if (currentArenaPattern[i][j]) {
					if (arenaTiles[i][j]) {
						ComponentBoxCollider* boxCollider = arenaTiles[i][j]->GetComponent<ComponentBoxCollider>();
						if (boxCollider && !boxCollider->IsActive()) {
							boxCollider->Enable();
						}
					}
				}
			}
		}

		if (timeRemainingTilesActive > 0.f) {
			timeRemainingTilesActive -= Time::GetDeltaTime();
		}
		else {
			//corridor
			for (int i = 0; i < CORRIDOR_ROWS; ++i) {
				for (int j = 0; j < CORRIDOR_COLS; ++j) {
					if (currentCorridorPattern[i][j]) {
						if (corridorTiles[i][j]) {
							ComponentBoxCollider* boxCollider = corridorTiles[i][j]->GetComponent<ComponentBoxCollider>();
							if (boxCollider && !boxCollider->IsActive()) {
								boxCollider->Disable();
							}
						}
					}
				}
			}

			//arena
			for (int i = 0; i < ARENA_ROWS; ++i) {
				for (int j = 0; j < ARENA_COLS; ++j) {
					if (currentArenaPattern[i][j]) {
						if (arenaTiles[i][j]) {
							ComponentBoxCollider* boxCollider = arenaTiles[i][j]->GetComponent<ComponentBoxCollider>();
							if (boxCollider && !boxCollider->IsActive()) {
								boxCollider->Disable();
							}
						}
					}
				}
			}
		}

	}

}