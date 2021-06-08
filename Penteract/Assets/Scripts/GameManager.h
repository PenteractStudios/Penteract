#pragma once

#include "GameSingleton.h"

#include "Scripting/Script.h"

class GameManager : public Script, public GameSingleton
{
	GENERATE_BODY(GameManager);

public:

	void Start() override;
	void Update() override;

public:
	UID gameManagerUID = 0;

	// Put the same values that in GameSingleton.h
	float volumeScene = 0.f;
};