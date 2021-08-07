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
		int i = 0;
		for (GameObject* children : corridor->GetChildren()) {
			corridorTiles[i] = children;
			++i;
		}
	}
	arena = GameplaySystems::GetGameObject(arenaUID);
	if (arena) {
		int i = 0;
		for (GameObject* children : arena->GetChildren()) {
			arenaTiles[i] = children;
			++i;
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
			currentCorridorPattern = corridorPattern1;
			currentArenaPattern = arenaPattern1;
			break;
		case 2:
			currentCorridorPattern = corridorPattern2;
			currentArenaPattern = arenaPattern2;
			break;
		case 3:
			currentCorridorPattern = corridorPattern3;
			currentArenaPattern = arenaPattern3;
			break;
		default:
			break;
		}
	}
	
	if (warningActive) {
		if (firstTimeWarning) {
			UpdateWarningTiles(true);
			firstTimeWarning = false;
		}

		if (timeRemainingWarning > 0.f) {
			timeRemainingWarning -= Time::GetDeltaTime();
		}
		else {
			UpdateWarningTiles(false);
			warningActive = false;
			fireActive = true;
			timeRemainingTilesActive = timeTilesActive;
			firstTimeFireActive = true;
		}
	}

	if (fireActive) {
		if (firstTimeFireActive) {
			UpdateFireActiveTiles(true);
			firstTimeFireActive = false;
		}

		if (timeRemainingTilesActive > 0.f) {
			timeRemainingTilesActive -= Time::GetDeltaTime();
		}
		else {
			UpdateFireActiveTiles(false);
			warningActive = true;
			fireActive = false;
			patternFinished = true;
			previousPattern = currentPattern;
			timeRemainingWarning = timeWarning;
			firstTimeWarning = true;
		}

	}

}

void FloorIsLava::UpdateWarningTiles(bool activate)
{
	//corridor
	for (int i = 0; i < CORRIDOR_TILES; ++i) {
		if (currentCorridorPattern[i]) {
			if (corridorTiles[i]) {
				if (activate) {
					//corridorTiles[i][j].activate warning particles
				}
				else {
					//corridorTiles[i][j].deactivate warning particles
				}
			}			
		}
	}

	//arena
	for (int i = 0; i < ARENA_TILES; ++i) {
		if (currentArenaPattern[i]) {
			if (arenaTiles[i]) {
				if (activate) {
					//arenaTiles[i][j].activate warning particles
				}
				else {
					//arenaTiles[i][j].deactivate warning particles
				}
			}
		}		
	}
}

void FloorIsLava::UpdateFireActiveTiles(bool activate)
{
	//corridor
	for (int i = 0; i < CORRIDOR_TILES; ++i) {
		if (currentCorridorPattern[i]) {
			if (corridorTiles[i]) {
				ComponentBoxCollider* boxCollider = corridorTiles[i]->GetComponent<ComponentBoxCollider>();
				if (boxCollider) {
					if (activate) boxCollider->Enable();
					else boxCollider->Disable();
				}
				GameObject* childFireParticlesObject = corridorTiles[i]->GetChild("FireVerticalParticleSystem");
				if (childFireParticlesObject) {
					ComponentParticleSystem* fireParticles = childFireParticlesObject->GetComponent<ComponentParticleSystem>();
					if (fireParticles){
						if (activate) fireParticles->PlayChildParticles();
						else fireParticles->StopChildParticles();
					}
				}
			}
		}		
	}

	//arena
	for (int i = 0; i < ARENA_TILES; ++i) {
		if (currentArenaPattern[i]) {
			if (arenaTiles[i]) {
				ComponentBoxCollider* boxCollider = arenaTiles[i]->GetComponent<ComponentBoxCollider>();
				if (boxCollider) {
					if (activate) boxCollider->Enable();
					else boxCollider->Disable();
				}
				GameObject* childFireParticlesObject = arenaTiles[i]->GetChild("FireVerticalParticleSystem");
				if (childFireParticlesObject) {
					ComponentParticleSystem* fireParticles = childFireParticlesObject->GetComponent<ComponentParticleSystem>();
					if (fireParticles) {
						if (activate) fireParticles->PlayChildParticles();
						else fireParticles->StopChildParticles();
					}
				}
			}
		}
	}	
}


