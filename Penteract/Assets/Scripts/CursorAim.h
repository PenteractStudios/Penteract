#pragma once

#include "Scripting/Script.h"
#include "PlayerController.h"
class GameObject;
class ComponentTransform;

class CursorAim : public Script
{
	GENERATE_BODY(CursorAim);

public:

	void Start() override;
	void Update() override;


public:
	UID onimaruLaserUID = 0;
	UID fangUID = 0;
	UID cameraUID = 0;
	ComponentTransform* lookAtMouseCameraComp = nullptr;
	GameObject* fang = nullptr;
	PlayerController* playerController = nullptr;
};

