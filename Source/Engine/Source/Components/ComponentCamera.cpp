#include "ComponentCamera.h"

#include "Globals.h"
#include "Application.h"
#include "GameObject.h"
#include "Components/ComponentTransform.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleCamera.h"

#include "imgui.h"
#include "debugdraw.h"
#include "Math/float3x3.h"
#include "Math/Quat.h"

#include "Utils/Leaks.h"

#define JSON_TAG_FRUSTRUM "Frustum"
#define JSON_TAG_POS "Pos"
#define JSON_TAG_UP "Up"
#define JSON_TAG_FRONT "Front"
#define JSON_TAG_NEAR_PLANE_DISTANCE "NearPlaneDistance"
#define JSON_TAG_FAR_PLANE_DISTANCE "FarPlaneDistance"
#define JSON_TAG_HORIZONTAL_FOV "HorizontalFov"
#define JSON_TAG_VERTICAL_FOV "VerticalFov"
#define JSON_TAG_CAMERA_SELECTED "CameraSelected"

void ComponentCamera::Init() {
	UpdateFrustum();
}

void ComponentCamera::Update() {
	UpdateFrustum();
}

void ComponentCamera::DrawGizmos() {
	if (App->camera->GetActiveFrustum() == &frustum) return; //TODO: Possible bug when adding more components (component pointer invalidates)

	if (IsActiveInHierarchy()) dd::frustum(frustum.ViewProjMatrix().Inverted(), dd::colors::White);
}

void ComponentCamera::OnEditorUpdate() {
	if (ImGui::Checkbox("Main Camera", &activeCamera)) {
		App->camera->ChangeActiveFrustum(frustum, activeCamera);
	}
	if (ImGui::Checkbox("Frustum Culling", &cullingCamera)) {
		App->camera->ChangeCullingFrustum(frustum, cullingCamera);
	}
	ImGui::Separator();

	vec front = frustum.Front();
	vec up = frustum.Up();
	ImGui::TextColored(App->editor->titleColor, "Frustum");
	ImGui::InputFloat3("Front", front.ptr(), "%.3f", ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat3("Up", up.ptr(), "%.3f", ImGuiInputTextFlags_ReadOnly);

	float nearPlane = frustum.NearPlaneDistance();
	float farPlane = frustum.FarPlaneDistance();
	if (ImGui::DragFloat("Near Plane", &nearPlane, 0.1f, 0.0f, farPlane, "%.2f")) {
		frustum.SetViewPlaneDistances(nearPlane, farPlane);
	}
	if (ImGui::DragFloat("Far Plane", &farPlane, 1.0f, nearPlane, inf, "%.2f")) {
		frustum.SetViewPlaneDistances(nearPlane, farPlane);
	}
	float fov = frustum.VerticalFov();
	if (ImGui::InputFloat("Field of View", &fov, 0.0F, 0.0F, "%.2f")) {
		frustum.SetHorizontalFovAndAspectRatio(fov, frustum.AspectRatio());
	}
}

void ComponentCamera::Save(JsonValue jComponent) const {
	JsonValue jFrustum = jComponent[JSON_TAG_FRUSTRUM];
	JsonValue jPos = jFrustum[JSON_TAG_POS];
	jPos[0] = frustum.Pos().x;
	jPos[1] = frustum.Pos().y;
	jPos[2] = frustum.Pos().z;
	JsonValue jUp = jFrustum[JSON_TAG_UP];
	jUp[0] = frustum.Up().x;
	jUp[1] = frustum.Up().y;
	jUp[2] = frustum.Up().z;
	JsonValue jFront = jFrustum[JSON_TAG_FRONT];
	jFront[0] = frustum.Front().x;
	jFront[1] = frustum.Front().y;
	jFront[2] = frustum.Front().z;
	jFrustum[JSON_TAG_NEAR_PLANE_DISTANCE] = frustum.NearPlaneDistance();
	jFrustum[JSON_TAG_FAR_PLANE_DISTANCE] = frustum.FarPlaneDistance();
	jFrustum[JSON_TAG_HORIZONTAL_FOV] = frustum.HorizontalFov();
	jFrustum[JSON_TAG_VERTICAL_FOV] = frustum.VerticalFov();

	jComponent[JSON_TAG_CAMERA_SELECTED] = activeCamera;
}

void ComponentCamera::Load(JsonValue jComponent) {
	JsonValue jFrustum = jComponent[JSON_TAG_FRUSTRUM];
	JsonValue jPos = jFrustum[JSON_TAG_POS];
	JsonValue jUp = jFrustum[JSON_TAG_UP];
	JsonValue jFront = jFrustum[JSON_TAG_FRONT];
	frustum.SetFrame(vec(jPos[0], jPos[1], jPos[2]), vec(jFront[0], jFront[1], jFront[2]), vec(jUp[0], jUp[1], jUp[2]));
	frustum.SetViewPlaneDistances(jFrustum[JSON_TAG_NEAR_PLANE_DISTANCE], jFrustum[JSON_TAG_FAR_PLANE_DISTANCE]);
	frustum.SetPerspective(jFrustum[JSON_TAG_HORIZONTAL_FOV], jFrustum[JSON_TAG_VERTICAL_FOV]);

	activeCamera = jComponent[JSON_TAG_CAMERA_SELECTED];
}

void ComponentCamera::DuplicateComponent(GameObject& owner) {
	ComponentCamera* component = owner.CreateComponent<ComponentCamera>();
	component->frustum = this->frustum;
}

void ComponentCamera::UpdateFrustum() {
	ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
	frustum.SetPos(transform->GetGlobalPosition());

	float3x3 rotationMatrix = float3x3::FromQuat(transform->GetGlobalRotation());
	frustum.SetFront(rotationMatrix * float3::unitZ);
	frustum.SetUp(rotationMatrix * float3::unitY);
}

Frustum ComponentCamera::BuildDefaultFrustum() const {
	Frustum newFrustum;
	newFrustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	newFrustum.SetViewPlaneDistances(0.1f, 200.0f);
	newFrustum.SetHorizontalFovAndAspectRatio(DEGTORAD * 90.0f, 1.3f);
	newFrustum.SetFront(vec::unitZ);
	newFrustum.SetUp(vec::unitY);
	newFrustum.SetPos(vec::zero);
	return newFrustum;
}

Frustum* ComponentCamera::GetFrustum() {
	return &frustum;
}
