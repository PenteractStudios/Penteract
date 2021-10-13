#pragma once

#include "Scripting/Script.h"

class AIDuke;
class ComponentAudioSource;
class ComponentTransform;
class ComponentMeshRenderer;

class DukeShield : public Script {
	GENERATE_BODY(DukeShield);

public:

	void Start() override;
	void Update() override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;
	void InitShield();
	void FadeShield();

	bool GetIsActive() { return isActive; }
	void SetIsActive(bool a) { isActive = a; }
public:

	UID dukeUID = 0;
	AIDuke* duke = nullptr;
	float shieldMaxScale = 0.9f;
	float growthSpeed = 2.0f;
	float fadeSpeed = 4.0f;
	float growthThreshold = 0.05f;
	float fadeThreshold = 0.05f;

private:
	bool isActive = false;
	ComponentAudioSource* audio;
	ComponentTransform* transform;
	ComponentMeshRenderer* mesh;
	enum class ShieldState { OFFLINE, GROWING, IDLE, FADING };
	ShieldState shieldState = ShieldState::OFFLINE;
};

