#pragma once

#include "Scripting/Script.h"

class PlayerController;
class ComponentAudioSource;
class ComponentBillboard;

class Shield : public Script
{
	GENERATE_BODY(Shield);

public:

	void Start() override;
	void Update() override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;
	void InitShield();
	void FadeShield();

	bool CanUse() { return currentAvailableCharges > 0; }
	bool GetIsActive() { return isActive; }
	void SetIsActive(bool a) { isActive = a; }
	int GetNumCharges() { return currentAvailableCharges; }
	bool NeedsRecharging() { return currentAvailableCharges != maxCharges; }
	void IncreaseCharge() { currentAvailableCharges = currentAvailableCharges == maxCharges ? currentAvailableCharges : currentAvailableCharges + 1; }
	float GetChargeCooldown() { return chargeCooldown; }

public:

	int maxCharges = 0;
	float chargeCooldown = 0;
	int currentAvailableCharges = 0;
	UID playerUID = 0;
	PlayerController* playerController = nullptr;
	ComponentBillboard* shieldBilb = nullptr;
	UID ShieldBilboardUID = 0;
	float maxFrames = 0;

private:
	bool isActive = false;
	ComponentAudioSource* audio = nullptr;
	float currentFrame = 0;
	float factor = 0;
};