#pragma once

#include "Scripting/Script.h"

class FloorIsLava : public Script
{
	GENERATE_BODY(FloorIsLava);

public:

	void Start() override;
	void Update() override;

public:
	UID corridorUID = 0;
	UID arenaUID = 0;

private:

	GameObject* corridor = nullptr;
	GameObject* arena = nullptr;

	GameObject* corridorTiles[2][18];
	GameObject* arenaTiles[8][5];

};

