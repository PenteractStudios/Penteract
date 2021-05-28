#pragma once

#include "Scripting/Script.h"

class GameObject;

class SpawnOnClick : public Script {
	GENERATE_BODY(SpawnOnClick);

public:
	UID cameraUID = 0;
	UID prefabId = 0;

public:
	void Start() override;
	void Update() override;

	void Spawn();

private:
	GameObject* gameObject = nullptr;
	GameObject* camera = nullptr;
	
private:	
	float3 DetectMouseLocation();
};
