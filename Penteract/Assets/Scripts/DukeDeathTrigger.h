#pragma once

#include "Scripting/Script.h"

class GameObject;
class VideoSceneEnd;

class DukeDeathTrigger : public Script
{
	GENERATE_BODY(DukeDeathTrigger);

public:

	void Start() override;
	void Update() override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;

public:
	UID videoCanvasUID = 0;

private:
	GameObject* videoCanvas = nullptr;
	VideoSceneEnd* videoSceneEndScript = nullptr;

	// Scene flow controls
	bool triggered = false;
	bool playVideo = false;

};

