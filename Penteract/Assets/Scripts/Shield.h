#pragma once

#include "Scripting/Script.h"

class Onimaru;

class Shield : public Script
{
	GENERATE_BODY(Shield);

public:

	void Start() override;
	void Update() override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance) override;
	void InitShield();
	void FadeShield();

	bool GetIsActive() { return isActive; }
	void SetIsActive(bool a) { isActive = a; }
	int GetCoolDown() { return totalCooldown; }
	int GetNumCharges() { return num_charges; }

public:

	UID onimaruUID = 0;
	int max_charges = 0;
	int cooldownCharge = 0;

	GameObject* onimaruGO = nullptr;
	Onimaru* onimaru = nullptr;

private:
	int num_charges = 0;
	int totalCooldown = 0;
	bool isActive = false;

};
