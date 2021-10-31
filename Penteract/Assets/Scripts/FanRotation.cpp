#include "FanRotation.h"

#include "GameObject.h"
#include "Components/ComponentTransform.h"
#include "GameplaySystems.h"

#define circle 2*pi

EXPOSE_MEMBERS(FanRotation) {
	MEMBER(MemberType::FLOAT, rotationSpeed),
	MEMBER(MemberType::BOOL, playReverse),
};

GENERATE_BODY_IMPL(FanRotation);

void FanRotation::Start() {
	transform = GetOwner().GetComponent<ComponentTransform>();
	if (transform) {
		currentDegree = transform->GetGlobalRotation().ToEulerXYZ().x * DEGTORAD;
	}
}

void FanRotation::Update() {
	if (transform) {
		float3 newRotation = transform->GetGlobalRotation().ToEulerXYZ();
		currentDegree += (playReverse ? 1 : -1) * pi * rotationSpeed * Time::GetDeltaTime();

		if (playReverse) {
			if (currentDegree > circle) {
				currentDegree -= circle;
			}
		}
		else {
			if (currentDegree < -circle) {
				currentDegree += circle;
			}
		}
		

		newRotation.x = currentDegree;
		newRotation.z = 0;
		
		transform->SetGlobalRotation(newRotation);
	}
}