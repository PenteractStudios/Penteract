#pragma once

#include "Scripting/Script.h"

#include "Math/Quat.h"

class LightShoot : public Script
{
	GENERATE_BODY(LightShoot);

public:

	void Start() override;
	void Update() override;
	void SetOnimaruDirection(Quat direction);
public:
	int speed = 200;
	float life = 0.2f;

private:
	float restLife = 0.0f;
	Quat onimaruDirection;
};
