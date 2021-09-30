#pragma once

#include "Scripting/Script.h"

class ComponentVideo;
class GameObject;
class CanvasFader;

class VideoScene1Start : public Script
{
	GENERATE_BODY(VideoScene1Start);

public:

	void Start() override;
	void Update() override;

public:
	UID canvasFaderUID = 0;
private: 
	void BackToNormalGameplay();

private:
	ComponentVideo* componentVideo = nullptr;
	GameObject* parent = nullptr;
	CanvasFader* faderScript = nullptr;
};

