#pragma once

#include "Scripting/Script.h"

class ComponentVideo;
class GameObject;

class VideoScene1Start : public Script
{
	GENERATE_BODY(VideoScene1Start);

public:

	void Start() override;
	void Update() override;

private:
	ComponentVideo* componentVideo = nullptr;
	GameObject* parent = nullptr;

};

