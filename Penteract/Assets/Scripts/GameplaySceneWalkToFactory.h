#pragma once

#include "Scripting/Script.h"
#include "AIState.h"

class GameObject;
class ComponentAgent;
class AIMovement;
class CameraController;
class DialogueManager;

class GameplaySceneWalkToFactory : public Script
{
	GENERATE_BODY(GameplaySceneWalkToFactory);

public:

	void Start() override;
	void Update() override;
	void OnCollision(GameObject& /* collidedWith */, float3 /* collisionNormal */, float3 /* penetrationDistance */, void* /* particle */ = nullptr) override;

public:
	UID duke2UID = 0;
	float3 dukeRunTowards = float3(-70.f, 3.f, 0.f);
	float dukeSpeed = 3.f;

	UID gameCameraUID = 0;
	float3 cameraNewPosition = float3(0.f, 0.f, 0.f);
	float cameraPanningTime = 0.f;
	float cameraMoveSpeed = 1.5f;

	UID laserBeamsSecurityUID = 0;

	UID gameControllerUID = 0;
	int dialogueID = 0;

private:
	GameObject* duke2 = nullptr;
	ComponentAgent* dukeAgent = nullptr;
	AIMovement* movementScript = nullptr;
	AIState state = AIState::RUN;

	GameObject* gameCamera = nullptr;
	CameraController* cameraControllerScript = nullptr;
	float elapsedPanningTime = 0.f;

	GameObject* laserBeamsSecurity = nullptr;

	GameObject* gameController = nullptr;
	DialogueManager* dialogueManagerScript = nullptr;

	// Scene flow controllers
	bool triggered = false;
	bool sceneStarted = false;
	bool finishScene = false;

	float storedCameraSmoothValue = 0;
};

