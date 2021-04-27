#pragma once

#include "Script.h"

class ExitButton : public Script
{
	GENERATE_BODY(ExitButton);

public:

	void Start() override;
	void Update() override;
	void OnButtonClick() override;
};
