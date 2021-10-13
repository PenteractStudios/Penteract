#include "BossLasersPatterns.h"

#include "GameplaySystems.h"
#include "LasersGenerator.h"

#include <algorithm>

EXPOSE_MEMBERS(BossLasersPatterns) {
	MEMBER(MemberType::FLOAT, laserActiveDuration),
	MEMBER(MemberType::FLOAT, intervalDuration),
	MEMBER(MemberType::GAME_OBJECT_UID, generatorA1UID),
	MEMBER(MemberType::GAME_OBJECT_UID, generatorB1UID),
	MEMBER(MemberType::GAME_OBJECT_UID, generatorC1UID),
	MEMBER(MemberType::GAME_OBJECT_UID, generatorD1UID)
};

GENERATE_BODY_IMPL(BossLasersPatterns);

void BossLasersPatterns::Start() {
	gen = std::minstd_rand(rd());

	GameObject* generatorA1 = nullptr;
	if(generatorA1UID) generatorA1 = GameplaySystems::GetGameObject(generatorA1UID);

	GameObject* generatorB1 = nullptr;
	if(generatorB1UID) generatorB1 = GameplaySystems::GetGameObject(generatorB1UID);

	GameObject* generatorC1 = nullptr;
	if(generatorC1UID) generatorC1 = GameplaySystems::GetGameObject(generatorC1UID);

	GameObject* generatorD1 = nullptr;
	if(generatorD1UID) generatorD1 = GameplaySystems::GetGameObject(generatorD1UID);
	
	float chargingTime = 0.f;
	if (generatorA1) {
		LasersGenerator* script = GET_SCRIPT(generatorA1, LasersGenerator);
		if (script) {
			generators.push_back(script);
			chargingTime = script->chargingDuration;
		}
	}
	if (generatorB1) {
		LasersGenerator* script = GET_SCRIPT(generatorB1, LasersGenerator);
		if (script) generators.push_back(script);
	}
	if (generatorC1) {
		LasersGenerator* script = GET_SCRIPT(generatorC1, LasersGenerator);
		if (script) generators.push_back(script);
	}
	if (generatorD1) {
		LasersGenerator* script = GET_SCRIPT(generatorD1, LasersGenerator);
		if (script) generators.push_back(script);
	}

	permutationDuration = intervalDuration * (generators.size() - 1) + chargingTime + laserActiveDuration;
	permutationTimer = permutationDuration;
}

void BossLasersPatterns::Update() {
	permutationTimer += Time::GetDeltaTime();
	if (permutationTimer > permutationDuration) {
		// Create a random permutation
		permutationTimer = 0;
		std::shuffle(generators.begin(), generators.end(), gen);

		// Set the times for this round
		for (unsigned i = 0; i < generators.size(); ++i) {
			generators[i]->coolDownOnTimer = 0.f;
			generators[i]->coolDownOffTimer = permutationDuration - intervalDuration * i;
			generators[i]->coolDownOn = laserActiveDuration;
			generators[i]->coolDownOff = permutationDuration;
			generators[i]->Init();
		}
	}
}