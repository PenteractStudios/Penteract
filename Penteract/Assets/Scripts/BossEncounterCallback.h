#pragma once

#include "Scripting/Script.h"

class AIDuke;

class BossEncounterCallback : public Script {
	GENERATE_BODY(BossEncounterCallback);

public:
	void Start() override;
	void Update() override;
	void Init();

public:
	UID dukeUID = 0;

private:
	GameObject* gameObject = nullptr;
	GameObject* duke = nullptr;
	AIDuke* dukeScript = nullptr;
	bool shouldTeleport = true;
};

