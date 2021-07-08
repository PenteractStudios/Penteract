#pragma once

#include "Scripting/Script.h"

#include "Geometry/Frustum.h"

class GameObject;
class ComponentCamera;
class ComponentTransform;
class StatsDisplayer;

struct TesseractEvent;

class GameController : public Script {
	GENERATE_BODY(GameController);

public:

	void Start() override;
	void Update() override;
	void ReceiveEvent(TesseractEvent& e) override;

	void Rotate(float2 mouseMotion, Frustum* frustum, ComponentTransform* transform);
	void PauseGame();
	void ResumeGame();

public:
	UID gameCameraUID = 0;
	UID godCameraUID = 0;
	UID staticCamera1UID = 0;
	UID staticCamera2UID = 0;
	UID staticCamera3UID = 0;
	UID staticCamera4UID = 0;
	UID playerUID = 0;

	UID pauseUID = 0;
	UID hudUID = 0;
	UID settingsPlusUID = 0;
	UID dialoguesUID = 0;
	UID statsDisplayerUID = 0;

	UID godModeControllerUID = 0;

	float speed = 50.f;
	float rotationSpeedX = 10.f;
	float rotationSpeedY = 10.f;
	float focusDistance = 100.f;
	float transitionSpeed = 1000.f;

private:
	void DoTransition();
	void SpawnEnemies();
	void ClearPauseMenus();
	void EnablePauseMenus();

private:
	GameObject* gameCamera = nullptr;
	GameObject* godCamera = nullptr;
	ComponentCamera* camera = nullptr;
	ComponentCamera* staticCamera1 = nullptr;
	ComponentCamera* staticCamera2 = nullptr;
	ComponentCamera* staticCamera3 = nullptr;
	ComponentCamera* staticCamera4 = nullptr;
	GameObject* player = nullptr;

	GameObject* pauseCanvas = nullptr;
	GameObject* hudCanvas = nullptr;
	GameObject* settingsCanvas = nullptr;
	GameObject* dialogueCanvas = nullptr;
	StatsDisplayer* statsController = nullptr;

	GameObject* godModeController = nullptr;

	float yaw = 0.f;
	float pitch = 0.f;
	bool showWireframe = false;
	bool godModeAvailable = false;
	bool transitionFinished = false;

	bool isPaused = false;
};

