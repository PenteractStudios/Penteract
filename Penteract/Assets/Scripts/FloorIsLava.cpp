#include "FloorIsLava.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "GlobalVariables.h"

#include <math.h>

EXPOSE_MEMBERS(FloorIsLava) {
	MEMBER_SEPARATOR("Normal Attributes"),
    MEMBER(MemberType::FLOAT, timeTilesActiveNormal),
	MEMBER(MemberType::FLOAT, timeWarning),
	MEMBER(MemberType::STRING, container),
	MEMBER(MemberType::BOOL, sequential),

	MEMBER_SEPARATOR("Boss Exclusive Attributes"),
	MEMBER(MemberType::FLOAT, timeTilesActiveInterphase)
};

GENERATE_BODY_IMPL(FloorIsLava);

void FloorIsLava::Start() {
	
	tiles = GetOwner().GetChildren();		
	
	if (container == "corridor") {
		sequentialCorridorPatterns = corridorPatterns;
	}
	else if (container == "arena") {
		pattern1 = arenaPattern1.pattern;
		pattern2 = arenaPattern2.pattern;
		pattern3 = arenaPattern3.pattern;
	}
	else if (container == "boss") {
		sequentialBossPatternsNormal = bossPatternsNormal;
		sequentialBossPatternsInterphase = bossPatternsInterphase;
	}
	
	timeTilesActive = timeTilesActiveNormal;
	timeRemainingTilesActive = timeTilesActive;
	timeRemainingWarning = timeWarning;	
}

void FloorIsLava::Update() {
	
	if (!started) return;

	//select a random corridor and arena pattern
	if (patternFinished) {
		if (sequential) {
			if (container == "boss") {
				if (!interphase) {
					SetSequentialPatterns(BOSS_PATTERNS_NORMAL, bossPatternsNormal);
				}
				else {
					SetSequentialPatterns(BOSS_PATTERNS_INTERPHASE, bossPatternsInterphase);
				}
			}
			else if (container == "corridor") {
				SetSequentialPatterns(CORRIDOR_PATTERNS, sequentialCorridorPatterns);
			}
		}
		else {
			currentPattern = nextPattern;
			while (currentPattern == nextPattern) {
				nextPattern = 1 + (rand() % 3);
			}
			SetRandomPattern(currentPattern, currentTilesPattern);
			SetRandomPattern(nextPattern, nextTilesPattern);
		}
		patternFinished = false;
	}
	
	if (warningActive) {
		if (firstTimeWarning) {
			UpdateWarningTiles();
			firstTimeWarning = false;
		}
		if (timeRemainingWarning > 0.f) {
			timeRemainingWarning -= Time::GetDeltaTime();
		}
		else {
			warningActive = false;
			fireActive = true;
		}		
	}

	if (fireActive) {

		if (GameplaySystems::GetGlobalVariable(globalIsGameplayBlocked, true)) return;

		if (firstTimeFireActive) {
			UpdateFireActiveTiles(true);
			firstTimeFireActive = false;
			firsTimeFireStopped = true;
		}

		if (timeRemainingTilesActive > 0.f) {
			timeRemainingTilesActive -= Time::GetDeltaTime();
			if (timeRemainingTilesActive <= std::min(timeWarning, timeTilesActive)) {
				if (firstTimeNextWarning) {
					UpdateWarningNextTiles();
					firstTimeNextWarning = false;
				}
			}
		}
		else {
			if (firsTimeFireStopped) {
				UpdateFireActiveTiles(false);
				firsTimeFireStopped = false;
			}
			patternFinished = true;
			firstTimeNextWarning = true;
			firstTimeFireActive = true;
			timeRemainingTilesActive = timeTilesActive;
		}
	}

}

void FloorIsLava::StartFire()
{
	started = true;
	timeRemainingWarning = timeWarning;
	warningActive = true;
	firstTimeWarning = true;
	fireActive = false;
	firstTimeFireActive = true;
	firsTimeFireStopped = false;
	if (interphase) {
		timeTilesActive = timeTilesActiveInterphase;
	}
	else {
		timeTilesActive = timeTilesActiveNormal;
	}
}

void FloorIsLava::StopFire()
{
	started = false;
	for (unsigned i = 0; i < tiles.size(); ++i) {
		if (tiles[i]) {
			ComponentBoxCollider* boxCollider = tiles[i]->GetComponent<ComponentBoxCollider>();
			GameObject* childFireParticlesObject = tiles[i]->GetChild("FireVerticalParticleSystem");
			ComponentAnimation* animation = tiles[i]->GetComponent<ComponentAnimation>();
			ComponentParticleSystem* fireParticles = nullptr;
			if (childFireParticlesObject) {
				fireParticles = childFireParticlesObject->GetComponent<ComponentParticleSystem>();
			}
			if (boxCollider && fireParticles && animation) {
				boxCollider->Disable();
				fireParticles->StopChildParticles();
				animation->SendTrigger(animation->GetCurrentState()->name + "Closed");
			}			
		}
	}
	if (sequential) sequentialCount = 0;
}

void FloorIsLava::SetRandomPattern(int pattern, const bool*& boolPattern)
{
	switch (pattern)
	{
	case 1:
		boolPattern = pattern1;
		break;
	case 2:
		boolPattern = pattern2;
		break;
	case 3:
		boolPattern = pattern3;
		break;
	default:
		break;
	}
}

void FloorIsLava::SetSequentialPatterns(int countSize, const TilesPattern* sequentialPattern)
{
	currentTilesPattern = sequentialPattern[sequentialCount].pattern;
	++sequentialCount;
	if (sequentialCount >= countSize) {
		sequentialCount = 0;
	}
	nextTilesPattern = sequentialPattern[sequentialCount].pattern;
}

void FloorIsLava::UpdateWarningTiles()
{
	for (unsigned i = 0; i < tiles.size(); ++i) {
		if (currentTilesPattern[i]) {
			if (tiles[i]) {
				ComponentAnimation* animation = tiles[i]->GetComponent<ComponentAnimation>();
				if (animation) {
					animation->SendTrigger("ClosedOpening");
				}				
			}			
		}
	}
}

void FloorIsLava::UpdateWarningNextTiles()
{
	for (unsigned i = 0; i < tiles.size(); ++i) {
		if (nextTilesPattern[i] && !currentTilesPattern[i]) {
			if (tiles[i]) {
				ComponentAnimation* animation = tiles[i]->GetComponent<ComponentAnimation>();
				if (animation) {
					animation->SendTrigger("ClosedOpening");
				}
			}
		}
	}
}

void FloorIsLava::UpdateFireActiveTiles(bool activate)
{
	for (unsigned i = 0; i < tiles.size(); ++i) {
		if (currentTilesPattern[i]) {
			if (tiles[i]) {
				ComponentBoxCollider* boxCollider = tiles[i]->GetComponent<ComponentBoxCollider>();
				GameObject* childFireParticlesObject = tiles[i]->GetChild("FireVerticalParticleSystem");
				ComponentAnimation* animation = tiles[i]->GetComponent<ComponentAnimation>();
				ComponentParticleSystem* fireParticles = nullptr;
				if (childFireParticlesObject) {
					fireParticles = childFireParticlesObject->GetComponent<ComponentParticleSystem>();					
				}
				if (boxCollider && fireParticles && animation) {
					if (activate) {
						boxCollider->Enable();
						fireParticles->PlayChildParticles();
					}
					else {
						if (!nextTilesPattern[i]) {
							boxCollider->Disable();
							fireParticles->StopChildParticles();
							animation->SendTrigger("OpenedClosing");
						}
					}
				}
			}
		}		
	}
}

void FloorIsLava::SetInterphase(bool interphaseActive)
{
	interphase = interphaseActive;
}


