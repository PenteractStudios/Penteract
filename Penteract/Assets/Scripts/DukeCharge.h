#pragma once

#include "Scripting/Script.h"

class AIDuke;

class DukeCharge : public Script
{
	GENERATE_BODY(DukeCharge);

public:

	void Start() override;
	void Update() override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;

public:

	UID dukeUID = 0;

private:

	AIDuke* aiDuke = nullptr;

};

