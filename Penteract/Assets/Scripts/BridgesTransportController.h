#pragma once

#include "Scripting/Script.h"

class GameObject;
class ResourcePrefab;
class ComponentTransform;
class ComponentAudioSource;

class BridgesTransportController : public Script
{
	GENERATE_BODY(BridgesTransportController);

public:
	/* GameObjects*/
	UID initialBridgeUID = 0;
	UID finalBridgeUID = 0;
	UID bridgeObstaclesUID = 0;

	bool hasToBeEnabledBridges = true;	// Status of the bridges
	bool isClosedBridges = false;		// If is closed the bridges
	float speedAnimationBridges = 0.1f;	// Speed
public:

	void Start() override;
	void Update() override;

	/* Enable the spawn points on trigger  */
	void OnCollision(GameObject& /* collidedWith */, float3 /* collisionNormal */, float3 penetrationDistance, void* particle = nullptr) override;

	/* Control the bridges */
	void OpenBridges();
	void CloseBridges();
	void MoveBridges();
private:
	GameObject* gameObject = nullptr;
	GameObject* childEncounter = nullptr;

	/* Bridges Transport Objects */
	GameObject* initialBridge = nullptr;
	GameObject* finalBridge = nullptr;
	GameObject* bridgeObstacles = nullptr;

	/* Transforms */
	ComponentTransform* transformInitialBridge = nullptr;
	ComponentTransform* transformFinalBridge = nullptr;

	/* Audios */
	ComponentAudioSource* audioInitialBridge = nullptr;
	ComponentAudioSource* audioFinalBridge = nullptr;
	bool audioPlayed = false;
};

