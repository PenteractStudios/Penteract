#include "AbilityRefreshEffect.h"

#include "GameplaySystems.h"
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentTransform2D.h"

// clang-format off
EXPOSE_MEMBERS(AbilityRefreshEffect) {
	MEMBER(MemberType::FLOAT, totalEffectTime),
	MEMBER(MemberType::FLOAT, effectScale),
	MEMBER(MemberType::GAME_OBJECT_UID, effectMember1UID),
	MEMBER(MemberType::GAME_OBJECT_UID, skillImageObjectUID),
	MEMBER(MemberType::BOOL, debugPlay)
};// clang-format on


GENERATE_BODY_IMPL(AbilityRefreshEffect);

void AbilityRefreshEffect::Start() {
	GameObject* effectMember1Obj = GameplaySystems::GetGameObject(effectMember1UID);
	GameObject* skillImageObject = GameplaySystems::GetGameObject(skillImageObjectUID);

	if (effectMember1Obj) {
		effectMember1 = effectMember1Obj->GetComponent<ComponentImage>();
		effectTransform2D = effectMember1Obj->GetComponent<ComponentTransform2D>();
	}

	if (skillImageObject) {
		skillImageTransform2D = skillImageObject->GetComponent<ComponentTransform2D>();
		skillImageObject->Disable();
	}
}



void AbilityRefreshEffect::Update() {

	if (debugPlay) {
		debugPlay = false;
		Play();
	}

	if (!isPlaying || !skillImageTransform2D) return;

	effectTimer = Min(totalEffectTime, effectTimer + Time::GetDeltaTime());

	//DO EFFECT
	skillImageTransform2D->SetScale(float3::Lerp(effectScaleVector, originalScaleVector, effectTimer / totalEffectTime));
	effectTransform2D->SetScale(float3::Lerp(effectScaleVector, originalEffectScaleVector, effectTimer / totalEffectTime));

	if (effectTimer == totalEffectTime) {
		isPlaying = false;
		effectMember1->Disable();
	}
}

void AbilityRefreshEffect::Play() {
	if (!skillImageTransform2D) return;
	effectMember1->Enable();
	originalScaleVector = skillImageTransform2D->GetScale();
	originalEffectScaleVector = effectTransform2D->GetScale();
	effectScaleVector = float3(effectScale);
	effectTimer = 0;
	isPlaying = true;
}