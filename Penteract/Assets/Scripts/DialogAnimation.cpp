#include "DialogAnimation.h"

#include "GameplaySystems.h"

#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentText.h"

EXPOSE_MEMBERS(DialogAnimation) {
	MEMBER(MemberType::FLOAT3, startPosition),
	MEMBER(MemberType::FLOAT3, endPosition),
	MEMBER(MemberType::FLOAT, appearAnimationTime),
	MEMBER(MemberType::FLOAT, disappearAnimationTime)
};

GENERATE_BODY_IMPL(DialogAnimation);

void DialogAnimation::Start() {
	RetrieveUIComponents(&GetOwner());
}

void DialogAnimation::Update() {
	if (!startAnimationFinished) {
		if (currentTime < appearAnimationTime) {
			GetOwner().GetComponent<ComponentTransform2D>()->SetPosition(float3::Lerp(startPosition, endPosition, currentTime / appearAnimationTime));
			TransitionUIElementsColor(true);
			currentTime += Time::GetDeltaTime();
		}
		else {
			startAnimationFinished = true;
			GetOwner().GetComponent<ComponentTransform2D>()->SetPosition(endPosition);
		}
	}
}

void DialogAnimation::TransitionUIElementsColor(bool fromTransparent)
{
	int i = 0;
	for (Component* component : uiComponents) {
		if (component->GetType() == ComponentType::IMAGE) {
			static_cast<ComponentImage*>(component)->SetColor(float4::Lerp(float4(uiColors[i].x, uiColors[i].y, uiColors[i].z, fromTransparent ? 0.0f : 1.0f), uiColors[i], currentTime / appearAnimationTime));
		}
		else {
			static_cast<ComponentText*>(component)->SetFontColor(float4::Lerp(float4(uiColors[i].x, uiColors[i].y, uiColors[i].z, fromTransparent ? 0.0f : 1.0f), uiColors[i], currentTime / appearAnimationTime));
		}
		i++;
	}
}

void DialogAnimation::RetrieveUIComponents(GameObject* current)
{
	if (current == nullptr) {
		return;
	}

	ComponentImage* image = current->GetComponent<ComponentImage>();
	ComponentText* text = current->GetComponent<ComponentText>();
	if (image) {
		uiComponents.push_back(static_cast<Component*>(image));
		uiColors.push_back(image->GetMainColor());
	}
	else if (text) {
		uiComponents.push_back(static_cast<Component*>(text));
		uiColors.push_back(text->GetFontColor());
	}
	for (GameObject* child : current->GetChildren()) {
		RetrieveUIComponents(child);
	}
}
