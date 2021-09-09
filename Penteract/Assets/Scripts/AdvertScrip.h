#pragma once

#include "Scripting/Script.h"


class AdvertScrip : public Script
{
	GENERATE_BODY(AdvertScrip);

public:

	void Start() override;
	void Update() override;

	float speed = 0.0f;
	float startTime = 0.0f;
	float changeTime = 0.0f;
	float restTime = 0.0f;

private:
	bool firstAdvert = true;
	bool isChanging = true;
	float2 offset = { 0.0f, 0.0f };
};

