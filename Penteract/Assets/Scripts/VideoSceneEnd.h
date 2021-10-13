#pragma once

#include "Scripting/Script.h"

class ComponentVideo;
class VideoSceneEnd : public Script
{
	GENERATE_BODY(VideoSceneEnd);

public:
	
	void Start() override;
	void Update() override;
	void PlayVideo();

public:
	UID winSceneUID = 0;

private: 
	void BackToNormalGameplay();
	ComponentVideo* componentVideo = nullptr;
	GameObject* parent = nullptr;

};

