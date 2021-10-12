#pragma once

#include "Scripting/Script.h"

class GameObject;
class ResourcePrefab;
class ComponentTransform;

class BridgesTransportController : public Script
{
	GENERATE_BODY(BridgesTransportController);

public:
	UID initialBridgeUID = 0;
	UID finalBridgeUID = 0;
	UID bridgeObstaclesUID = 0;
	bool hasToBeEnabledBridges = true;
	bool isTransportArea = false;
	float speedAnimationBridges = 0.1f;
public:

	void Start() override;
	void Update() override;

	/* Enable the spawn points on trigger  */
	void OnCollision(GameObject& /* collidedWith */, float3 /* collisionNormal */, float3 penetrationDistance, void* particle = nullptr) override;

	void OpenBridges();
	void CloseBridges();
	void MoveBridges();
private:
	/* Owner */
	GameObject* gameObject = nullptr;

	/* Bridges Transport Object */
	GameObject* initialBridge = nullptr;
	GameObject* finalBridge = nullptr;
	GameObject* bridgeObstacles = nullptr;

	ComponentTransform* transformInitialBridge = nullptr;
	ComponentTransform* transformFinalBridge = nullptr;
};

