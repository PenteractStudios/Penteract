#pragma once

#include "Scripting/Script.h"

class ComponentAudioSource;
class ComponentAnimation;

class FactoryDoors : public Script {
	GENERATE_BODY(FactoryDoors);

public:
	void Start() override;
	void Update() override;
	void Open();

private:
	ComponentAnimation* animation = nullptr;
	ComponentAudioSource* audio = nullptr;
	bool isOpen = false;
};
