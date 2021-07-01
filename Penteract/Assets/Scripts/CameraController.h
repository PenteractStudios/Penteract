#pragma once

#include "Scripting/Script.h"

class CameraController : public Script
{
	GENERATE_BODY(CameraController);

public:

	void Start() override;
	void Update() override;

};

