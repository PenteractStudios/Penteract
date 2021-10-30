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
	MEMBER(MemberType::FLOAT, timeTilesActiveInterphase),

	MEMBER_SEPARATOR("Sound Positioning"),
	MEMBER(MemberType::GAME_OBJECT_UID, playerUID),
	MEMBER(MemberType::FLOAT, soundDistance),
	MEMBER(MemberType::GAME_OBJECT_UID, fireAudioUID)
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

	// Get audio sources
	audiosFireTilesOpening.resize(tiles.size());
	audiosFireTilesClosing.resize(tiles.size());
	
	for (unsigned i = 0; i < tiles.size(); ++i) {
		int j = 0;
		for (ComponentAudioSource& audioComp : tiles[i]->GetComponents<ComponentAudioSource>()) {
			if (j == (int) Audios::OPEN) {
				audiosFireTilesOpening.push_back(&audioComp);
			}
			else if (j == (int) Audios::CLOSE) {
				audiosFireTilesClosing.push_back(&audioComp);
			} 
			j++;
		}
	}

	playerGameObject = GameplaySystems::GetGameObject(playerUID);
	fireAudioGameObject = GameplaySystems::GetGameObject(fireAudioUID);
}

void FloorIsLava::Update() {
	
	if (!started) return;

	if (playerGameObject) {
		playerPosition = playerGameObject->GetComponent<ComponentTransform>()->GetGlobalPosition();
	}

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

	if (fireAudioGameObject) {
		std::vector<GameObject*> children = fireAudioGameObject->GetChildren();
		for (GameObject* gameObject : children) {
			if (gameObject) {
				ComponentAudioSource* audio = gameObject->GetComponent<ComponentAudioSource>();
				if (audio) audio->Play();
			}
		}
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

	if (fireAudioGameObject) {
		std::vector<GameObject*> children = fireAudioGameObject->GetChildren();
		for (GameObject* gameObject : children) {
			if (gameObject) {
				ComponentAudioSource* audio = gameObject->GetComponent<ComponentAudioSource>();
				if (audio) audio->Stop();
			}
		}
	}
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
					animation->SendTrigger(animation->GetCurrentState()->name + "Opening");
				}

				ComponentTransform* tileTransform = tiles[i]->GetComponent<ComponentTransform>();
				if (tileTransform) {
					float3 position = tileTransform->GetGlobalPosition();
					if (Distance(position, playerPosition) < soundDistance) {
						GameObject* audioOpening = tiles[i]->GetChild("AudioOpening");
						ComponentAudioSource* audioOpeningComp = nullptr;
						if (audioOpening) audioOpeningComp = audioOpening->GetComponent<ComponentAudioSource>();
						if (audioOpeningComp) {
							audioOpeningComp->Play();
						}
					}
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
					animation->SendTrigger(animation->GetCurrentState()->name + "Opening");
				}

				ComponentTransform* tileTransform = tiles[i]->GetComponent<ComponentTransform>();
				if (tileTransform) {
					float3 position = tileTransform->GetGlobalPosition();
					if (Distance(position, playerPosition) < soundDistance) {
						GameObject* audioOpening = tiles[i]->GetChild("AudioOpening");
						ComponentAudioSource* audioOpeningComp = nullptr;
						if (audioOpening) audioOpeningComp = audioOpening->GetComponent<ComponentAudioSource>();
						if (audioOpeningComp) {
							audioOpeningComp->Play();
						}
					}
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
							animation->SendTrigger(animation->GetCurrentState()->name + "Closing");

							ComponentTransform* tileTransform = tiles[i]->GetComponent<ComponentTransform>();
							if (tileTransform) {
								float3 position = tileTransform->GetGlobalPosition();
								if (Distance(position, playerPosition) < soundDistance) {
									GameObject* audioClosing = tiles[i]->GetChild("AudioClosing");
									ComponentAudioSource* audioClosingComp = nullptr;
									if (audioClosing) audioClosingComp = audioClosing->GetComponent<ComponentAudioSource>();
									if (audioClosingComp) {
										audioClosingComp->Play();
									}
								}
							}
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


