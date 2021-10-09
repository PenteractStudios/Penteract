#pragma once

#include "Scripting/Script.h" 

class MovingLasers;

class LaserTriggerLvl2 : public Script
{
	GENERATE_BODY(LaserTriggerLvl2);

public:
	UID laserUID = 0;
	bool SwitchOn = true;
public:

	void Start() override;
	void Update() override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;


private:
	MovingLasers* laserScript = nullptr;
};

