#include "FactoryDoors.h"

#include "GameObject.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(FactoryDoors) {};

GENERATE_BODY_IMPL(FactoryDoors);

void FactoryDoors::Start() {
	Debug::Log("Start");
	animation = GetOwner().GetComponent<ComponentAnimation>();
}

void FactoryDoors::Update() {
	if (!animation) return;

	if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_G)) {
		Open();
	}
}

void FactoryDoors::Open() {
	animation->SendTrigger("ClosedOpening");
	Debug::Log("Opening");
}