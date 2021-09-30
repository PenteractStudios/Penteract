#pragma once

#include "Scripting/Script.h"

class ComponentVideo;
class GameObject;
class DialogueManager;
class CanvasFader;

class VideoScene1Start : public Script
{
	GENERATE_BODY(VideoScene1Start);

public:

	void Start() override;
	void Update() override;

public:
	UID canvasFaderUID = 0;
	UID gameControllerUID = 0;
	int dialogueID = 0;

private: 
	void BackToNormalGameplay();
	ComponentVideo* componentVideo = nullptr;
	GameObject* parent = nullptr;
	GameObject* gameController = nullptr;
	DialogueManager* dialogueManagerScript = nullptr;
	CanvasFader* faderScript = nullptr;
};

