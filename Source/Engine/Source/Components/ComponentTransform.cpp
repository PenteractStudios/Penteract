#include "ComponentTransform.h"

#include "Globals.h"
#include "Application.h"
#include "GameObject.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentBoundingBox.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModuleCamera.h"

#include "Math/float3x3.h"
#include "SDL.h"

#include "Utils/Leaks.h"

#define JSON_TAG_POSITION "Position"
#define JSON_TAG_ROTATION "Rotation"
#define JSON_TAG_SCALE "Scale"
#define JSON_TAG_LOCAL_EULER_ANGLES "LocalEulerAngles"

void ComponentTransform::OnEditorUpdate() {
	float3 pos = position;
	float3 scl = scale;
	float3 rot = localEulerAngles;

	ImGui::TextColored(App->editor->titleColor, "Transformation (X,Y,Z)");
	if (ImGui::DragFloat3("Position", pos.ptr(), App->editor->dragSpeed2f, -inf, inf)) {
		SetPosition(pos);
	}
	if (ImGui::DragFloat3("Scale", scl.ptr(), App->editor->dragSpeed2f, 0.0001f, inf, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
		SetScale(scl);
	}

	if (ImGui::DragFloat3("Rotation", rot.ptr(), App->editor->dragSpeed2f, -inf, inf)) {
		SetRotation(rot);
	}
}

void ComponentTransform::Save(JsonValue jComponent) const {
	JsonValue jPosition = jComponent[JSON_TAG_POSITION];
	jPosition[0] = position.x;
	jPosition[1] = position.y;
	jPosition[2] = position.z;

	JsonValue jRotation = jComponent[JSON_TAG_ROTATION];
	jRotation[0] = rotation.x;
	jRotation[1] = rotation.y;
	jRotation[2] = rotation.z;
	jRotation[3] = rotation.w;

	JsonValue jScale = jComponent[JSON_TAG_SCALE];
	jScale[0] = scale.x;
	jScale[1] = scale.y;
	jScale[2] = scale.z;

	JsonValue jLocalEulerAngles = jComponent[JSON_TAG_LOCAL_EULER_ANGLES];
	jLocalEulerAngles[0] = localEulerAngles.x;
	jLocalEulerAngles[1] = localEulerAngles.y;
	jLocalEulerAngles[2] = localEulerAngles.z;
}

void ComponentTransform::Load(JsonValue jComponent) {
	JsonValue jPosition = jComponent[JSON_TAG_POSITION];
	position.Set(jPosition[0], jPosition[1], jPosition[2]);

	JsonValue jRotation = jComponent[JSON_TAG_ROTATION];
	rotation.Set(jRotation[0], jRotation[1], jRotation[2], jRotation[3]);

	JsonValue jScale = jComponent[JSON_TAG_SCALE];
	scale.Set(jScale[0], jScale[1], jScale[2]);

	JsonValue jLocalEulerAngles = jComponent[JSON_TAG_LOCAL_EULER_ANGLES];
	localEulerAngles.Set(jLocalEulerAngles[0], jLocalEulerAngles[1], jLocalEulerAngles[2]);

	dirty = true;
}

void ComponentTransform::DuplicateComponent(GameObject& owner) {
	ComponentTransform* component = owner.CreateComponent<ComponentTransform>();
	component->SetPosition(this->GetPosition());
	component->SetRotation(this->GetRotation());
	component->SetScale(this->GetScale());
}

void ComponentTransform::InvalidateHierarchy() {
	if (!dirty) {
		dirty = true;
		ComponentBoundingBox* boundingBox = GetOwner().GetComponent<ComponentBoundingBox>();
		if (boundingBox) boundingBox->Invalidate();

		for (GameObject* child : GetOwner().GetChildren()) {
			ComponentTransform* childTransform = child->GetComponent<ComponentTransform>();
			if (childTransform != nullptr) {
				childTransform->InvalidateHierarchy();
			}
		}
	}
}

void ComponentTransform::CalculateGlobalMatrix(bool force) {
	if (force || dirty) {
		localMatrix = float4x4::FromTRS(position, rotation, scale);

		GameObject* parent = GetOwner().GetParent();
		if (parent != nullptr) {
			ComponentTransform* parentTransform = parent->GetComponent<ComponentTransform>();

			parentTransform->CalculateGlobalMatrix();
			globalMatrix = parentTransform->globalMatrix * localMatrix;
			globalMatrix.Orthogonalize3();	// Solution for non-uniform scaled objects
		} else {
			globalMatrix = localMatrix;
		}

		dirty = false;
	}
}

void ComponentTransform::SetPosition(float3 position_) {
	position = position_;
	InvalidateHierarchy();
}

void ComponentTransform::SetRotation(Quat rotation_) {
	rotation = rotation_;
	localEulerAngles = rotation_.ToEulerXYZ().Mul(RADTODEG);
	InvalidateHierarchy();
}

void ComponentTransform::SetRotation(float3 rotation_) {
	rotation = Quat::FromEulerXYZ(rotation_.x * DEGTORAD, rotation_.y * DEGTORAD, rotation_.z * DEGTORAD);
	localEulerAngles = rotation_;
	InvalidateHierarchy();
}

void ComponentTransform::SetScale(float3 scale_) {
	scale = scale_;
	InvalidateHierarchy();
}

void ComponentTransform::SetTRS(float4x4& newTransform_) {
	position = newTransform_.Col3(3);
	newTransform_.Orthogonalize3();
	scale = float3(newTransform_.Col3(0).Length(), newTransform_.Col3(1).Length(), newTransform_.Col3(2).Length());
	newTransform_.Orthonormalize3();
	rotation = Quat(newTransform_.SubMatrix(3, 3));
	localEulerAngles = rotation.ToEulerXYZ().Mul(RADTODEG);
	InvalidateHierarchy();
}

float3 ComponentTransform::GetPosition() const {
	return position;
}

Quat ComponentTransform::GetRotation() const {
	return rotation;
}

float3 ComponentTransform::GetScale() const {
	return scale;
}

float3 ComponentTransform::GetGlobalPosition() {
	CalculateGlobalMatrix();
	return globalMatrix.TranslatePart();
}

Quat ComponentTransform::GetGlobalRotation() {
	CalculateGlobalMatrix();
	float4x4 newTransform_ = globalMatrix;
	newTransform_.Orthogonalize3();
	newTransform_.Orthonormalize3();
	return Quat(newTransform_.SubMatrix(3, 3));
}

float3 ComponentTransform::GetGlobalScale() {
	CalculateGlobalMatrix();
	return globalMatrix.GetScale();
}

const float4x4& ComponentTransform::GetLocalMatrix() {
	CalculateGlobalMatrix();
	return localMatrix;
}

const float4x4& ComponentTransform::GetGlobalMatrix() {
	CalculateGlobalMatrix();
	return globalMatrix;
}