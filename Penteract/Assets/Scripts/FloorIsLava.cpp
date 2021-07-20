#include "FloorIsLava.h"

#include "GameObject.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(FloorIsLava) {
    MEMBER(MemberType::GAME_OBJECT_UID, corridorUID),
    MEMBER(MemberType::GAME_OBJECT_UID, arenaUID)

};

GENERATE_BODY_IMPL(FloorIsLava);

void FloorIsLava::Start() {
	
	corridor = GameplaySystems::GetGameObject(corridorUID);
	if (corridor) {
		int i, j = 0;
		for (GameObject* children : corridor->GetChildren()) {
			if (j >= (sizeof(corridorTiles[0]) / sizeof(corridorTiles[0][0]))) {
				j = 0;
				if (i < sizeof(corridorTiles)/sizeof(corridorTiles[0])) ++i;
			}
			corridorTiles[i][j] = children;
			++j;
		}
	}
	arena = GameplaySystems::GetGameObject(arenaUID);
	if (arena) {
		int i, j = 0;
		for (GameObject* children : arena->GetChildren()) {
			if (j >= (sizeof(arenaTiles[0]) / sizeof(arenaTiles[0][0]))) {
				j = 0;
				if (i < sizeof(arenaTiles) / sizeof(arenaTiles[0])) ++i;
			}
			arenaTiles[i][j] = children;
			++j;
		}
	}
}

void FloorIsLava::Update() {
	
}