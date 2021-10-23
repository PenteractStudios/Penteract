#pragma once

#include "Scripting/Script.h"

class GameObject;
class FactoryDoors;
class FloorIsLava;
class ComponentParticleSystem;
class ComponentAudioSource;

class BridgeDoorButton : public Script
{
	GENERATE_BODY(BridgeDoorButton);

public:
	UID bridgeDoorUID = 0;
	UID fireBridgeUID = 0;
	UID fireArenaUID = 0;
	UID doorObstacleUID = 0;
	UID laserDoorObstacleUID = 0;
	UID buttonGlassParticlesObjUID = 0;

	float openDoorTime = 0;
	float closeButtonTime = 0;

public:

	void Start() override;
	void Update() override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;

private:
	GameObject* gameObject = nullptr;
	GameObject* doorObstacle = nullptr;
	GameObject* laserDoorObstacle = nullptr;
	ComponentAudioSource* laserDoorAudio = nullptr;
	FactoryDoors* doorScript = nullptr;
	FloorIsLava* bridgeTilesScript = nullptr;
	FloorIsLava* arenaTilesScript = nullptr;
	ComponentParticleSystem* buttonParticles = nullptr;
	ComponentParticleSystem* glassParticles = nullptr;
	
	float elapsedTime = 0;
	float elapsedButtonTime = 0;
	bool openedDoor = false;
	bool moveButton = false;
	bool audioButtonPlayed = false;
	float3 initialPosition = float3(0, 0, 0);
	bool fireEnabled = false;
};

