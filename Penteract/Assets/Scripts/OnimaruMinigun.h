#pragma once

#include "Scripting/Script.h"

class CameraController;
class ComponentCamera;
class ComponentTransform;
//class Player;

class OnimaruMinigun : public Script
{
	GENERATE_BODY(OnimaruMinigun);

public:

	void Start() override;
	void Update() override;

	
	
public:
	ComponentCamera* lookAtMouseCameraComp = nullptr;
	float3 lookAtMousePlanePosition = float3(0.f, 0.f, 0.f);
	float3 facePointDir = float3(0.f, 0.f, 0.f);
	ComponentTransform* playerMainTransform = nullptr;

public:
	GameObject* player = nullptr;
	/*Player* player = nullptr;*/
};

