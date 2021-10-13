#include "FactoryDoors.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "Components/ComponentAudioSource.h"

EXPOSE_MEMBERS(FactoryDoors) {};

GENERATE_BODY_IMPL(FactoryDoors);

void FactoryDoors::Start() {
	animation = GetOwner().GetComponent<ComponentAnimation>();
	audio = GetOwner().GetComponent<ComponentAudioSource>();
}

void FactoryDoors::Update() {
	
}

void FactoryDoors::Open() {
	if (!isOpen){
		if (animation) animation->SendTrigger("ClosedOpening");
		if (audio) audio->Play();
		isOpen =true;
	}
}