#pragma once

#include "Scripting/Script.h"

class GameObject;

class SpawnPointController : public Script {
	GENERATE_BODY(SpawnPointController);

public:
	void Start() override;
	void Update() override;
	void OnCollision(GameObject& collidedWith) override;

private:
	GameObject* gameObject = nullptr;
};
