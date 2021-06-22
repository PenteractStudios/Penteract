#pragma once

#include "Scripting/Script.h"

class MeleePunch : public Script
{
	GENERATE_BODY(MeleePunch);

public:

	void Start() override;
	void Update() override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance) override;
public:
	float life = 0.2f;
};

