#include "FactoryDoors.h"

#include "GameObject.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(FactoryDoors) {};

GENERATE_BODY_IMPL(FactoryDoors);

void FactoryDoors::Start() {
	animation = GetOwner().GetComponent<ComponentAnimation>();
	if(animation) Debug::Log("Start");
}

void FactoryDoors::Update() {
	if (!animation) return;

	if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_H)) {
		Debug::Log("Opening");
		animation->SendTrigger("ClosedOpening");
	}
}

void FactoryDoors::Open() {
	animation->SendTrigger("ClosedOpening");
}