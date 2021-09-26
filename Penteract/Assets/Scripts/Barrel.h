#pragma once

#include "Scripting/Script.h"

class ComponentSphereCollider;
class ComponentParticleSystem;
class ComponentAudioSource;
class ComponentTransform;

class CameraController;
class ResourcePrefab;
class Barrel : public Script
{
	GENERATE_BODY(Barrel);

public:

	void Start() override;
	void Update() override;

	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle) override;

public:
	UID barrelUID = 0;
	UID cameraUID = 0;
	UID particlesUID = 0;
	UID particlesForTimerUID = 0;
	UID sphereColliderUID = 0;

	GameObject* barrelCollider = nullptr;
	CameraController* cameraController = nullptr;
	ComponentParticleSystem* particles = nullptr;
	ComponentParticleSystem* particlesForTimer = nullptr;
	GameObject* barrel = nullptr;
	GameObject* barrelMesh = nullptr;
	ComponentAudioSource* audio = nullptr;
	ComponentAudioSource* audioForTimer = nullptr;
	bool isHit = false;
	bool destroy = false;
	float timeToDestroy = 1.0f;
	float timeWillDoDamage = 0.5f;

	float timerToDestroy = 3.0f;
	float currentTimerToDestroy = 0.0f;
	bool startTimerToDestroy = false;
	bool timerDestroyActivated = false;
	float shakeMultiplier = 1.85f;

	bool onFloor = true;	//Boolean to say if the barrel is on the ground (normal behavior) or is thrown by the boos
	float forceOfFall = 0.25f;

private:
	ComponentTransform* parentTransform = nullptr;
};
