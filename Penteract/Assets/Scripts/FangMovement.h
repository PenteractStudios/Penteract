#pragma once

#include "Scripting/Script.h"
#include "Player.h"

class GameObject;
class AIMovement;

class FangMovement : public Script
{
	GENERATE_BODY(FangMovement);

public:

	void Start() override;
	void Update() override;

public:
	UID cameraUID = 0;
	UID fangUID = 0;
	UID hitGOUID = 0;
	float distanceRayCast = 20;
	float cameraXPosition = -163;

private:
	GameObject* camera = nullptr;
	GameObject* fang = nullptr;
	GameObject* hitGo = nullptr;
	Player character = Player(7, 10.0f, 3, 0.1f, 1.0f);
};
