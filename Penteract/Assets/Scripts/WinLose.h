#pragma once

#include "Scripting/Script.h"

class GameObject;

class WinLose : public Script
{
	GENERATE_BODY(WinLose);

public:

	void Start() override;
	void Update() override;
	void KillEnemy();

public:
	UID winUID;
	UID playerUID;
	UID enemiesUID;

	float LoseOffsetX = 2.0f;
	float LoseOffsetZ = 2.0f;
	int totalEnemies = 0;

private:
	GameObject* winCon = nullptr;
	GameObject* player = nullptr;
	GameObject* enemies = nullptr;
	int deadEnemies = 0;

};
