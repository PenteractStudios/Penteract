#pragma once

#include "Scripting/Script.h"

class GameObject;
class ComponentAudioSource;
class PlayerController;
class DialogueManager;
class CameraController;
class VideoSceneEnd;

class DukeDeathTrigger : public Script
{
	GENERATE_BODY(DukeDeathTrigger);

public:

	void Start() override;
	void Update() override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;

public:
	UID playerUID = 0;
	UID dukeUID = 0;
	UID gameControllerUID = 0;
	UID gameCameraUID = 0;
	UID canvasHudUID = 0;
	UID videoCanvasUID = 0;
	UID audioControllerUID = 0;
	UID audioVideoSourceUID = 0;

	float relaxTime = 1.75f;
	float talkingDistance = 10.f;
	int dialogueID = 0;

private:
	GameObject* videoCanvas = nullptr;
	GameObject* duke = nullptr;
	PlayerController* playerController = nullptr;
	DialogueManager* dialogueManagerScript = nullptr;
	CameraController* camera = nullptr;
	VideoSceneEnd* videoSceneEndScript = nullptr;

	// Audio
	ComponentAudioSource* music = nullptr;
	ComponentAudioSource* audioVideo = nullptr;

	// Scene flow controls
	bool triggered = false;
	bool sceneStart = false;
	bool startDialogue = false;
	bool playVideo = false;

	float elapsedRelaxTime = 0;
	float3 talkPosition = float3(0, 0, 0);
};

