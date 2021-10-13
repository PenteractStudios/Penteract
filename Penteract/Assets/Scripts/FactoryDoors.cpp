#include "FactoryDoors.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentTransform.h"

EXPOSE_MEMBERS(FactoryDoors) {
	MEMBER(MemberType::FLOAT, speed),
	MEMBER(MemberType::FLOAT, yEndPos)
};

GENERATE_BODY_IMPL(FactoryDoors);

void FactoryDoors::Start() {
	audio = GetOwner().GetComponent<ComponentAudioSource>();
	transform = GetOwner().GetComponent<ComponentTransform>();
	float3 pos = transform->GetGlobalPosition();
	endPos = float3(pos.x, yEndPos, pos.z);
}

void FactoryDoors::Update() {
	if (isOpen) {
		float3 pos = float3::Lerp(transform->GetGlobalPosition(), endPos, speed * Time::GetDeltaTime());
		transform->SetGlobalPosition(pos);
		if (pos.y == endPos.y) isOpen = false;
	}
}

void FactoryDoors::Open() {
	if (!isOpen){
		if (audio) audio->Play();
		isOpen = true;
	}
}