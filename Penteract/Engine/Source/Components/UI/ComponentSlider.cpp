#include "ComponentSlider.h"

#include "GameObject.h"
#include "Application.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleUserInterface.h"
#include "Modules/ModuleWindow.h"
#include "imgui.h"
#include "Utils/Logging.h"
#include "Scripting/Script.h"

#include "Utils/Leaks.h"

#define JSON_TAG_COLOR_CLICK "ColorClick"
#define JSON_TAG_MAX_VALUE "MaxValue"
#define JSON_TAG_MIN_VALUE "MinValue"
#define JSON_TAG_CURRENT_VALUE "CurrentValue"
#define JSON_TAG_DIRECTION "Direction"
#define JSON_TAG_STOP_EDGES "StopOnEdges"

ComponentSlider::~ComponentSlider() {
}

void ComponentSlider::Init() {
	// TODO: Refactor this. It's not good right now but it lets me check the functionality
	std::vector<GameObject*> children = GetOwner().GetChildren();
	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it) {
		if (it == children.begin()) {
			background = *it;
		} else if (it == children.end() - 1) {
			handle = *it;
		} else {
			fill = *it;
		}
	}
	SetNormalizedValue();
}

void ComponentSlider::Update() {
	if (clicked) {
		if (!App->input->GetMouseButton(1)) {
			clicked = false;
		} else {
			float2 mousePos = App->input->GetMousePosition(true);
			#if GAME
			mousePos -= float2(App->window->GetPositionX(), App->window->GetPositionY());
			#endif
			ComponentCanvas* canvas = GetOwner().GetComponent<ComponentCanvasRenderer>()->AnyParentHasCanvas(&GetOwner());
			float2 auxNewPosition = float2(((mousePos.x - (App->renderer->GetViewportSize().x / 2.0f)) / canvas->GetScreenFactor()) - GetOwner().GetComponent<ComponentTransform2D>()->GetScreenPosition().x, 0);
			if (newPosition.x != auxNewPosition.x) {
				newPosition = auxNewPosition;
				OnValueChanged();
			}
		}
	}

	ComponentTransform2D* backgroundTransform = background->GetComponent<ComponentTransform2D>();
	ComponentTransform2D* fillTransform = fill->GetComponent<ComponentTransform2D>();
	ComponentTransform2D* handleTransform = handle->GetComponent<ComponentTransform2D>();

	// Calculate fill area and position

	float fillPosition = 0;
	float handlePosition = 0;

	handleTransform->SetSize(float2(backgroundTransform->GetSize().x / 8.0f, backgroundTransform->GetSize().y));

	switch (direction) {
	case DirectionType::LEFT_TO_RIGHT:
		fillTransform->SetSize(float2(backgroundTransform->GetSize().x * normalizedValue, backgroundTransform->GetSize().y));
		fillPosition = backgroundTransform->GetPosition().x - backgroundTransform->GetSize().x / 2.0f + (backgroundTransform->GetSize().x * normalizedValue) / 2.0f;
		handlePosition = fillTransform->GetPosition().x + (fillTransform->GetSize().x / 2.0f);
		break;
	case DirectionType::RIGHT_TO_LEFT:
		fillTransform->SetSize(float2(backgroundTransform->GetSize().x * (1 - normalizedValue), backgroundTransform->GetSize().y));
		fillPosition = backgroundTransform->GetPosition().x + backgroundTransform->GetSize().x / 2.0f - (backgroundTransform->GetSize().x * (1 - normalizedValue)) / 2.0f;
		handlePosition = fillTransform->GetPosition().x - (fillTransform->GetSize().x / 2.0f);
		break;
	// TODO
	/*case DirectionType::BOTTOM_TO_TOP:
			break;
		case DirectionType::TOP_TO_BOTTOM:
			break;*/
	default:
		break;
	}

	if (handleStopsOnEdge) {
		if (handlePosition + handleTransform->GetSize().x / 2.0f > backgroundTransform->GetPosition().x + backgroundTransform->GetSize().x / 2.0f) {
			handlePosition = backgroundTransform->GetPosition().x + backgroundTransform->GetSize().x / 2.0f - handleTransform->GetSize().x / 2.0f;
		} else if (handlePosition - handleTransform->GetSize().x / 2.0f < backgroundTransform->GetPosition().x - backgroundTransform->GetSize().x / 2.0f) {
			handlePosition = backgroundTransform->GetPosition().x - backgroundTransform->GetSize().x / 2.0f + handleTransform->GetSize().x / 2.0f;
		}
	}

	fillTransform->SetPosition(float3(fillPosition, backgroundTransform->GetPosition().y, backgroundTransform->GetPosition().z));
	handleTransform->SetPosition(float3(handlePosition, backgroundTransform->GetPosition().y, backgroundTransform->GetPosition().z));
}

void ComponentSlider::OnEditorUpdate() {
	if (ImGui::Checkbox("Active", &active)) {
		if (GetOwner().IsActive()) {
			if (active) {
				Enable();
			} else {
				Disable();
			}
		}
	}
	ImGui::Separator();

	ImGui::Checkbox("Handle Stops On Edges ", &handleStopsOnEdge);

	if (ImGui::DragFloat("Max. Value", &maxValue, App->editor->dragSpeed1f, minValue, inf)) {
		currentValue = currentValue > maxValue ? maxValue : currentValue;
		SetNormalizedValue();
	}
	if (ImGui::DragFloat("Min. Value", &minValue, App->editor->dragSpeed1f, -inf, maxValue)) {
		currentValue = currentValue < minValue ? minValue : currentValue;
		SetNormalizedValue();
	}

	const char* availableDirections[] = {"Left to right", "Right to left"};
	const char* currentDirection = availableDirections[(int) direction];
	if (ImGui::BeginCombo("Direction", currentDirection)) {
		for (int n = 0; n < IM_ARRAYSIZE(availableDirections); ++n) {
			bool isSelected = (currentDirection == availableDirections[n]);
			if (ImGui::Selectable(availableDirections[n], isSelected)) {
				// TODO: Implement calculation to let the slider go up->down / down->up
				direction = DirectionType(n);
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::SliderFloat("Value", &currentValue, minValue, maxValue)) {
		SetNormalizedValue();
	}
}

void ComponentSlider::OnClicked() {
	SetClicked(true);
	ComponentCanvas* canvas = GetOwner().GetComponent<ComponentCanvasRenderer>()->AnyParentHasCanvas(&GetOwner());
	App->userInterface->GetCurrentEventSystem()->SetSelected(GetOwner().GetComponent<ComponentSelectable>()->GetID());
	float2 mousePos = App->input->GetMousePosition(true);
	newPosition.x = ((mousePos.x - (App->renderer->GetViewportSize().x / 2.0f)) / canvas->GetScreenFactor()) - GetOwner().GetComponent<ComponentTransform2D>()->GetScreenPosition().x;

	OnValueChanged();
}

void ComponentSlider::OnValueChanged() {
	// TODO: support for vertical sliders
	ComponentTransform2D* backgroundTransform = background->GetComponent<ComponentTransform2D>();

	float size = 0.f;
	if (newPosition.x > backgroundTransform->GetPosition().x - backgroundTransform->GetSize().x / 2.0f) {
		size = newPosition.x - (backgroundTransform->GetPosition().x - backgroundTransform->GetSize().x / 2.0f);
		if (size > backgroundTransform->GetSize().x) {
			size = backgroundTransform->GetSize().x;
		}
	}

	normalizedValue = size / backgroundTransform->GetSize().x;
	currentValue = (maxValue - minValue) * normalizedValue;

	for (ComponentScript& scriptComponent : GetOwner().GetComponents<ComponentScript>()) {
		Script* script = scriptComponent.GetScriptInstance();
		if (script != nullptr) {
			script->OnValueChanged();
		}
	}
}

void ComponentSlider::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_MIN_VALUE] = minValue;
	jComponent[JSON_TAG_MAX_VALUE] = maxValue;
	jComponent[JSON_TAG_CURRENT_VALUE] = currentValue;
	jComponent[JSON_TAG_DIRECTION] = (int) direction;
	jComponent[JSON_TAG_STOP_EDGES] = handleStopsOnEdge;

	JsonValue jColorClick = jComponent[JSON_TAG_COLOR_CLICK];
	jColorClick[0] = colorClicked.x;
	jColorClick[1] = colorClicked.y;
	jColorClick[2] = colorClicked.z;
	jColorClick[3] = colorClicked.w;
}

void ComponentSlider::Load(JsonValue jComponent) {
	maxValue = jComponent[JSON_TAG_MAX_VALUE];
	minValue = jComponent[JSON_TAG_MIN_VALUE];
	currentValue = jComponent[JSON_TAG_CURRENT_VALUE];
	int dir = jComponent[JSON_TAG_DIRECTION];
	direction = (DirectionType) dir;
	handleStopsOnEdge = jComponent[JSON_TAG_STOP_EDGES];

	JsonValue jColorClick = jComponent[JSON_TAG_COLOR_CLICK];
	colorClicked = float4(jColorClick[0], jColorClick[1], jColorClick[2], jColorClick[3]);
}

bool ComponentSlider::IsClicked() const {
	return clicked;
}

void ComponentSlider::SetClicked(bool clicked_) {
	clicked = clicked_;
}

float4 ComponentSlider::GetClickColor() const {
	return colorClicked;
}

float2 ComponentSlider::GetClickedPosition() const {
	return newPosition;
}

float ComponentSlider::GetCurrentValue() const {
	return currentValue;
}

float ComponentSlider::GetMaxValue() const {
	return maxValue;
}

float ComponentSlider::GetMinValue() const {
	return minValue;
}

float ComponentSlider::GetNormalizedValue() const {
	return normalizedValue;
}

float4 ComponentSlider::GetTintColor() const {
	if (!IsActive()) return App->userInterface->GetErrorColor();

	ComponentSelectable* sel = GetOwner().GetComponent<ComponentSelectable>();

	if (!sel) return App->userInterface->GetErrorColor();

	if (sel->GetTransitionType() == ComponentSelectable::TransitionType::COLOR_CHANGE) {
		if (!sel->IsInteractable()) {
			return sel->GetDisabledColor();
		} else if (IsClicked()) {
			return colorClicked;
		} else if (sel->IsSelected()) {
			return sel->GetSelectedColor();
		} else if (sel->IsHovered()) {
			return sel->GetHoverColor();
		}
	}

	return App->userInterface->GetErrorColor();
}

void ComponentSlider::SetNormalizedValue() {
	if (maxValue - minValue == 0)
		normalizedValue = 0;
	else
		normalizedValue = (currentValue - minValue) / (maxValue - minValue);
}