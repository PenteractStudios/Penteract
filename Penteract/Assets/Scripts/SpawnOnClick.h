#pragma once

#include "Scripting/Script.h"

class GameObject;
class ResourcePrefab;

class SpawnOnClick : public Script {
	GENERATE_BODY(SpawnOnClick);

public:
	UID cameraUID = 0;
	UID prefabUID = 0;
	UID enemiesUID = 0;

public:
	void Start() override;
	void Update() override;

private:
	GameObject* gameObject = nullptr;
	GameObject* camera = nullptr;
	GameObject* enemies = nullptr;
	ResourcePrefab* prefab = nullptr;
	
private:	
	float3 DetectMouseLocation();
};
