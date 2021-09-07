#pragma once

#include "Scripting/Script.h"

class CursorAim : public Script
{
	GENERATE_BODY(CursorAim);

public:

	void Start() override;
	void Update() override;

};

