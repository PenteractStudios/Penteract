#include "HealthLostFeedback.h"
#include "GameplaySystems.h"

#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentTransform2D.h"

EXPOSE_MEMBERS(HealthLostFeedback) {
	MEMBER(MemberType::FLOAT, growthDuration),
	MEMBER(MemberType::FLOAT, growthMaxScale),
	MEMBER(MemberType::FLOAT, fadeOutDuration),
	MEMBER(MemberType::GAME_OBJECT_UID, imageUID)

};

GENERATE_BODY_IMPL(HealthLostFeedback);

void HealthLostFeedback::Play() {
	playing = true;
	growthTimer = 0;
	fadeOutTimer = 0;
}

void HealthLostFeedback::Start() {
	GameObject* obj = GameplaySystems::GetGameObject(imageUID);

	if (obj) {

		image = obj->GetComponent<ComponentImage>();
		transform = obj->GetComponent <ComponentTransform2D>();
	}

	originalSize = transform->GetSize();
}

void HealthLostFeedback::Update() {

	if (!transform || !image)return;

	if (playing) {
		if (growthTimer < growthDuration) {
			growthTimer = Min(growthTimer + Time::GetDeltaTime(), growthDuration);

			float delta = growthTimer / growthDuration;

			float scaleValue = Lerp(1, growthMaxScale, delta);
			transform->SetSize(float2(originalSize.x * scaleValue, originalSize.y * scaleValue));
			//Grow
		} else if (fadeOutTimer < fadeOutDuration) {
			fadeOutTimer = Min(fadeOutTimer + Time::GetDeltaTime(), fadeOutDuration);

			float delta = fadeOutTimer / fadeOutDuration;
			float fadeValue = Lerp(1, 0, delta);

			image->SetColor(float4(1, 0, 0, fadeValue));
			//FadeOut
		} else {
			playing = false;
			image->Disable();
			//Disable
		}
	}
}