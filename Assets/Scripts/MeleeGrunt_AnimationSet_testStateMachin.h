#pragma once

#include "Scripting/Script.h"

class ComponentAnimation;
class State;
struct TesseractEvent;

class MeleeGrunt_AnimationSet_testStateMachin : public Script
{
	GENERATE_BODY(MeleeGrunt_AnimationSet_testStateMachin);

public:

	void Start() override;
	void Update() override;
	void ReceiveEvent(TesseractEvent& e) override;

private:
	ComponentAnimation* animation = nullptr;
	State* currentState = nullptr;
};

