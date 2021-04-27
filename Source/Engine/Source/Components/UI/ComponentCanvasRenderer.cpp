#include "ComponentCanvasRenderer.h"

#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentText.h"
#include "Components/UI/ComponentCanvas.h"
#include "Components/UI/ComponentTransform2D.h"
#include "Components/UI/ComponentCanvas.h"
#include "GameObject.h"

#include "Utils/Leaks.h"

void ComponentCanvasRenderer::Save(JsonValue jComponent) const {
}

void ComponentCanvasRenderer::Load(JsonValue jComponent) {
}

void ComponentCanvasRenderer::Render(const GameObject* gameObject) const {
	ComponentTransform2D* transform2D = gameObject->GetComponent<ComponentTransform2D>();
	const ComponentCanvas* parentCanvas = AnyParentHasCanvas(&GetOwner());
	if (transform2D != nullptr && parentCanvas != nullptr) { // Get the Parent in a variable if needed and add canvas customization to render

		//IF OTHER COMPONENTS THAT RENDER IN UI ARE IMPLEMENTED, THEY MUST HAVE THEIR DRAW METHODS CALLED HERE
		ComponentImage* componentImage = gameObject->GetComponent<ComponentImage>();
		if (componentImage != nullptr) {
			componentImage->Draw(transform2D);
		}

		ComponentText* componentText = gameObject->GetComponent<ComponentText>();
		if (componentText != nullptr) {
			componentText->Draw(transform2D);
		}
	}
}

float ComponentCanvasRenderer::GetCanvasScreenFactor() const {
	return AnyParentHasCanvas(&GetOwner())->GetScreenFactor();
}

void ComponentCanvasRenderer::DuplicateComponent(GameObject& owner) {
	ComponentCanvasRenderer* component = owner.CreateComponent<ComponentCanvasRenderer>();
}

const ComponentCanvas* ComponentCanvasRenderer::AnyParentHasCanvas(GameObject* current) const {
	ComponentCanvas* currentCanvas = current->GetComponent<ComponentCanvas>();
	if (currentCanvas != nullptr) {
		return currentCanvas;
	} else {
		if (current->GetParent() != nullptr) {
			return AnyParentHasCanvas(current->GetParent());
		}
	}

	return nullptr;
}
