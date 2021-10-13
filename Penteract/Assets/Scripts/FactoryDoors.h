#pragma once

#include "Scripting/Script.h"

class ComponentAudioSource;
class ComponentTransform;

class FactoryDoors : public Script {
	GENERATE_BODY(FactoryDoors);

public:
	void Start() override;
	void Update() override;
	void Open();

public: 
	float speed = 1.0f;
	float yEndPos = -9.0f;

private:
	ComponentAudioSource* audio = nullptr;
	bool isOpen = false;
	ComponentTransform* transform = nullptr;
	float3 endPos;
};
