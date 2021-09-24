#pragma once

#include "Scripting/Script.h"

class GameObject;
class FactoryDoors;
class FloorIsLava;

class BridgeDoorButton : public Script
{
	GENERATE_BODY(BridgeDoorButton);

public:
	UID bridgeDoorUID = 0;
	UID fireBridgeUID = 0;
	UID fireArenaUID = 0;

public:

	void Start() override;
	void Update() override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;

private:
	GameObject* gameObject = nullptr;
	FactoryDoors* doorScript = nullptr;
	FloorIsLava* bridgeTilesScript = nullptr;
	FloorIsLava* arenaTilesScript = nullptr;
};

