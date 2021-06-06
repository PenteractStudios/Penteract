#pragma once

#include "Scripting/Script.h"

// Class to control all the principal variables of the game
class GameManager : public Script
{
	GENERATE_BODY(GameManager);

public:

	void Start() override;
	void Update() override;

public:
	GameObject* gameManager = nullptr;
	UID gameManagerUID = 0;
	// Maybe we have to use Getters & Setters
	float volumeScene = 0.f;

private:
	void Awake();
};

