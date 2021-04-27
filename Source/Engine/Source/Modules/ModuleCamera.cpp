#include "ModuleCamera.h"

#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "Utils/MSTimer.h"
#include "GameObject.h"
#include "Components/ComponentType.h"
#include "Components/ComponentBoundingBox.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentTransform.h"
#include "Resources/ResourceMesh.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModuleWindow.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleEditor.h"

#include "Math/float3.h"
#include "Math/float3x3.h"
#include "Math/float4x4.h"
#include "Geometry/Sphere.h"
#include "Geometry/LineSegment.h"
#include "Geometry/Triangle.h"
#include "SDL_mouse.h"
#include "SDL_scancode.h"
#include "SDL_video.h"
#include "Brofiler.h"
#include <vector>

#include "Utils/Leaks.h"

static void WarpMouseOnEdges() {
	const float2& mousePosition = App->input->GetMousePosition();

	SDL_DisplayMode displayMode;
	int displayIndex = SDL_GetWindowDisplayIndex(App->window->window);
	SDL_GetCurrentDisplayMode(displayIndex, &displayMode);
	SDL_Rect displayBounds;
	SDL_GetDisplayBounds(displayIndex, &displayBounds);
	int screenWidth = displayMode.w;
	int screenHeight = displayMode.h;

	if (mousePosition.x < displayBounds.x + 20) {
		App->input->WarpMouse(displayBounds.x + displayBounds.w - 22, (int) mousePosition.y);
	}
	if (mousePosition.y < displayBounds.y + 20) {
		App->input->WarpMouse((int) mousePosition.x, displayBounds.y + displayBounds.h - 22);
	}
	if (mousePosition.x > displayBounds.x + displayBounds.w - 20) {
		App->input->WarpMouse(displayBounds.x + 22, (int) mousePosition.y);
	}
	if (mousePosition.y > displayBounds.y + displayBounds.h - 20) {
		App->input->WarpMouse((int) mousePosition.x, displayBounds.y + 22);
	}
}

bool ModuleCamera::Init() {
	activeFrustum->SetKind(FrustumSpaceGL, FrustumRightHanded);
	activeFrustum->SetViewPlaneDistances(0.1f, 2000.0f);
#if GAME
	float ar = (float) App->window->GetWidth() / (float) App->window->GetHeight();
	activeFrustum->SetHorizontalFovAndAspectRatio(DEGTORAD * 90.0f, ar);
#else
	activeFrustum->SetHorizontalFovAndAspectRatio(DEGTORAD * 90.0f, 1.3f);
#endif
	activeFrustum->SetFront(vec::unitZ);
	activeFrustum->SetUp(vec::unitY);

	SetPosition(vec(2, 3, -5));
	LookAt(0, 0, 0);

	return true;
}

UpdateStatus ModuleCamera::Update() {
	BROFILER_CATEGORY("ModuleCamera - Update", Profiler::Color::Blue)

	if (activeFrustum != &engineCameraFrustum) return UpdateStatus::CONTINUE;

	float deltaTime = App->time->GetRealTimeDeltaTime();

	const float2& mouseMotion = App->input->GetMouseMotion();

	// Increase zoom and movement speed with shift
	float finalMovementSpeed = movementSpeed;
	float finalZoomSpeed = zoomSpeed;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) || App->input->GetKey(SDL_SCANCODE_RSHIFT)) {
		finalMovementSpeed *= shiftMultiplier;
		finalZoomSpeed *= shiftMultiplier;
	}

	// Zoom with mouse wheel
	float mouseWheelMotion = App->input->GetMouseWheelMotion();
	if (mouseWheelMotion < -FLT_EPSILON || mouseWheelMotion > FLT_EPSILON) {
		Zoom(mouseWheelMotion * 0.1f * focusDistance);
	}

	if (App->input->GetKey(SDL_SCANCODE_LALT)) {
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT)) {
			WarpMouseOnEdges();

			// Orbit with alt + left mouse button
			vec oldFocus = activeFrustum->Pos() + activeFrustum->Front().Normalized() * focusDistance;
			Rotate(float3x3::RotateAxisAngle(activeFrustum->WorldRight().Normalized(), -mouseMotion.y * rotationSpeed * DEGTORAD));
			Rotate(float3x3::RotateY(-mouseMotion.x * rotationSpeed * DEGTORAD));
			vec newFocus = activeFrustum->Pos() + activeFrustum->Front().Normalized() * focusDistance;
			Translate(oldFocus - newFocus);
		} else if (App->input->GetMouseButton(SDL_BUTTON_RIGHT)) {
			WarpMouseOnEdges();

			// Zoom with alt + right mouse button
			Zoom(mouseMotion.y * finalZoomSpeed * focusDistance);
		}
	} else if (App->input->GetMouseButton(SDL_BUTTON_RIGHT)) {
		WarpMouseOnEdges();

		// Rotate with mouse motion
		Rotate(float3x3::RotateAxisAngle(activeFrustum->WorldRight().Normalized(), -mouseMotion.y * rotationSpeed * DEGTORAD));
		Rotate(float3x3::RotateY(-mouseMotion.x * rotationSpeed * DEGTORAD));

		// Move with WASD + QE
		if (App->input->GetKey(SDL_SCANCODE_Q)) {
			Translate(activeFrustum->Up().Normalized() * -finalMovementSpeed * focusDistance * deltaTime);
		}
		if (App->input->GetKey(SDL_SCANCODE_E)) {
			Translate(activeFrustum->Up().Normalized() * finalMovementSpeed * focusDistance * deltaTime);
		}
		if (App->input->GetKey(SDL_SCANCODE_W)) {
			Translate(activeFrustum->Front().Normalized() * finalMovementSpeed * focusDistance * deltaTime);
		}
		if (App->input->GetKey(SDL_SCANCODE_S)) {
			Translate(activeFrustum->Front().Normalized() * -finalMovementSpeed * focusDistance * deltaTime);
		}
		if (App->input->GetKey(SDL_SCANCODE_A)) {
			Translate(activeFrustum->WorldRight().Normalized() * -finalMovementSpeed * focusDistance * deltaTime);
		}
		if (App->input->GetKey(SDL_SCANCODE_D)) {
			Translate(activeFrustum->WorldRight().Normalized() * finalMovementSpeed * focusDistance * deltaTime);
		}
	} else {
		// Focus camera around geometry with f key
		if (App->input->GetKey(SDL_SCANCODE_F)) {
			Focus(App->editor->selectedGameObject);
		}

		// Move with arrow keys
		if (App->input->GetKey(SDL_SCANCODE_UP)) {
			Translate(activeFrustum->Front().Normalized() * finalMovementSpeed * focusDistance * deltaTime);
		}
		if (App->input->GetKey(SDL_SCANCODE_DOWN)) {
			Translate(activeFrustum->Front().Normalized() * -finalMovementSpeed * focusDistance * deltaTime);
		}
		if (App->input->GetKey(SDL_SCANCODE_LEFT)) {
			Translate(activeFrustum->WorldRight().Normalized() * -finalMovementSpeed * focusDistance * deltaTime);
		}
		if (App->input->GetKey(SDL_SCANCODE_RIGHT)) {
			Translate(activeFrustum->WorldRight().Normalized() * finalMovementSpeed * focusDistance * deltaTime);
		}
	}

	return UpdateStatus::CONTINUE;
}

void ModuleCamera::CalculateFrustumNearestObject(float2 pos) {
	MSTimer timer;
	timer.Start();

	if (activeFrustum != &engineCameraFrustum) return;

	std::vector<GameObject*> intersectingObjects;
	LineSegment ray = engineCameraFrustum.UnProjectLineSegment(pos.x, pos.y);

	// Check with AABB
	Scene* scene = App->scene->scene;
	for (GameObject& gameObject : scene->gameObjects) {
		gameObject.flag = false;
		if (gameObject.isInQuadtree) continue;

		ComponentBoundingBox* boundingBox = gameObject.GetComponent<ComponentBoundingBox>();
		if (boundingBox == nullptr) continue;

		const AABB& gameObjectAABB = boundingBox->GetWorldAABB();
		if (ray.Intersects(gameObjectAABB)) {
			intersectingObjects.push_back(&gameObject);
		}
	}
	if (scene->quadtree.IsOperative()) {
		GetIntersectingAABBRecursive(scene->quadtree.root, scene->quadtree.bounds, ray, intersectingObjects);
	}

	GameObject* selectedGameObject = nullptr;
	float minDistance = inf;
	float distance = 0;
	for (GameObject* gameObject : intersectingObjects) {
		for (ComponentMeshRenderer& mesh : gameObject->GetComponents<ComponentMeshRenderer>()) {
			ResourceMesh* meshResource = App->resources->GetResource<ResourceMesh>(mesh.meshId);
			if (meshResource == nullptr) continue;

			const float4x4& model = gameObject->GetComponent<ComponentTransform>()->GetGlobalMatrix();
			std::vector<Triangle> triangles = meshResource->ExtractTriangles(model);
			for (Triangle& triangle : triangles) {
				if (!ray.Intersects(triangle, &distance, NULL)) continue;
				if (distance >= minDistance) continue;

				selectedGameObject = gameObject;
				minDistance = distance;
			}
		}
	}

	if (selectedGameObject != nullptr) {
		App->editor->selectedGameObject = selectedGameObject;
	}

	LOG("Ray Tracing in %ums", timer.Stop());
}

void ModuleCamera::ChangeActiveFrustum(Frustum& frustum, bool change) {
	if (change) {
		activeFrustum = &frustum;
	} else {
		activeFrustum = &engineCameraFrustum;
	}
}

void ModuleCamera::ChangeCullingFrustum(Frustum& frustum, bool change) {
	if (change) {
		cullingFrustum = &frustum;
	} else {
		cullingFrustum = &engineCameraFrustum;
	}
}

void ModuleCamera::CalculateFrustumPlanes() {
	float3 pos = cullingFrustum->Pos();
	float3 up = cullingFrustum->Up().Normalized();
	float3 front = cullingFrustum->Front();
	float3 right = cullingFrustum->WorldRight().Normalized();
	float farDistance = cullingFrustum->FarPlaneDistance();
	float nearDistance = cullingFrustum->NearPlaneDistance();
	float aspectRatio = cullingFrustum->AspectRatio();
	float vFov = cullingFrustum->VerticalFov();

	float hFar = 2 * tan(vFov / 2) * farDistance;
	float wFar = hFar * aspectRatio;
	float hNear = 2 * tan(vFov / 2) * nearDistance;
	float wNear = hNear * aspectRatio;
	float3 farCenter = pos + front * farDistance;
	float3 nearCenter = pos + front * nearDistance;

	frustumPlanes.points[0] = farCenter + (up * hFar / 2) - (right * wFar / 2);
	frustumPlanes.points[1] = farCenter + (up * hFar / 2) + (right * wFar / 2);
	frustumPlanes.points[2] = farCenter - (up * hFar / 2) - (right * wFar / 2);
	frustumPlanes.points[3] = farCenter - (up * hFar / 2) + (right * wFar / 2);
	frustumPlanes.points[4] = nearCenter + (up * hNear / 2) - (right * wNear / 2);
	frustumPlanes.points[5] = nearCenter + (up * hNear / 2) + (right * wNear / 2);
	frustumPlanes.points[6] = nearCenter - (up * hNear / 2) - (right * wNear / 2);
	frustumPlanes.points[7] = nearCenter - (up * hNear / 2) + (right * wNear / 2);

	frustumPlanes.planes[0] = cullingFrustum->LeftPlane();
	frustumPlanes.planes[1] = cullingFrustum->RightPlane();
	frustumPlanes.planes[2] = cullingFrustum->TopPlane();
	frustumPlanes.planes[3] = cullingFrustum->BottomPlane();
	frustumPlanes.planes[4] = cullingFrustum->FarPlane();
	frustumPlanes.planes[5] = cullingFrustum->NearPlane();
}

bool ModuleCamera::IsEngineCameraActive() const {
	if (activeFrustum == &engineCameraFrustum) {
		return true;
	}
	return false;
}

void ModuleCamera::Translate(const vec& translation) {
	activeFrustum->SetPos(activeFrustum->Pos() + translation);
}

void ModuleCamera::Zoom(float amount) {
	Translate(activeFrustum->Front().Normalized() * amount);
	focusDistance -= amount;
	if (focusDistance < 0.0f) {
		focusDistance = 0.0f;
	}
}

void ModuleCamera::Rotate(const float3x3& rotationMatrix) {
	vec oldFront = activeFrustum->Front().Normalized();
	vec oldUp = activeFrustum->Up().Normalized();
	activeFrustum->SetFront(rotationMatrix * oldFront);
	activeFrustum->SetUp(rotationMatrix * oldUp);
}

void ModuleCamera::LookAt(float x, float y, float z) {
	vec direction = vec(x, y, z) - activeFrustum->Pos();
	focusDistance = direction.Length();
	direction.Normalize();
	vec up = vec::unitY;

	// Special case for when looking straight up
	if (direction.Cross(up).IsZero()) {
		up = vec::unitZ;
	}

	Rotate(float3x3::LookAt(activeFrustum->Front().Normalized(), direction, activeFrustum->Up().Normalized(), up));
}

void ModuleCamera::Focus(const GameObject* gameObject) {
	if (gameObject == nullptr) {
		// Focus origin
		SetPosition(float3::zero - GetFront() * 30.f);
	} else {
		// Focus a GameObject
		if (gameObject->HasComponent<ComponentMeshRenderer>()) {
			// If the GO has Mesh, focus on that mesh
			ComponentBoundingBox* boundingBox = gameObject->GetComponent<ComponentBoundingBox>();
			if (!boundingBox) return;
			const AABB& worldBoundingBox = boundingBox->GetWorldAABB();
			if (!worldBoundingBox.IsFinite()) return;

			Sphere boundingSphere = worldBoundingBox.MinimalEnclosingSphere();
			float minHalfAngle = Min(activeFrustum->HorizontalFov(), activeFrustum->VerticalFov()) * 0.5f;
			float relativeDistance = boundingSphere.r / Sin(minHalfAngle);
			vec cameraDirection = -activeFrustum->Front().Normalized();
			vec cameraPosition = boundingSphere.pos + (cameraDirection * relativeDistance);
			vec modelCenter = boundingSphere.pos;
			SetPosition(cameraPosition);
			LookAt(modelCenter.x, modelCenter.y, modelCenter.z);
		} else {
			// If it doesn't have Mesh, focus on its children's Meshes. (Looks down all the hierarchy)
			float3 modelCenter;
			float distance;
			if (gameObject->HasChildren()) {
				modelCenter = gameObject->GetComponent<ComponentTransform>()->GetGlobalPosition();
				float3 minPoint = float3::inf, maxPoint = -float3::inf;
				CalculateExtremePointsRecursive(gameObject, minPoint, maxPoint);
				if (minPoint.IsFinite() && maxPoint.IsFinite()) {
					distance = (maxPoint - minPoint).Length();
				} else {
					distance = 30.f;
				}
			} else {
				// But if it doesn't have children, simply return its position as center and the default distance
				modelCenter = gameObject->GetComponent<ComponentTransform>()->GetGlobalPosition();
				distance = 30.f;
			}
			SetPosition(modelCenter - GetFront() * distance);
			focusDistance = (modelCenter - activeFrustum->Pos()).Length();
		}
	}
}

void ModuleCamera::CalculateExtremePointsRecursive(const GameObject* gameObject, float3& minPoint, float3& maxPoint) {
	for (GameObject* child : gameObject->GetChildren()) {
		if (child->HasComponent<ComponentMeshRenderer>()) {
			ComponentBoundingBox* childBoundingBox = child->GetComponent<ComponentBoundingBox>();
			if (childBoundingBox->GetWorldOBB().MinimalEnclosingAABB().IsFinite()) {
				minPoint = minPoint.Min(childBoundingBox->GetWorldAABB().minPoint);
				maxPoint = maxPoint.Max(childBoundingBox->GetWorldAABB().maxPoint);
			}
		}
		if (child->HasChildren()) CalculateExtremePointsRecursive(child, minPoint, maxPoint);
	}
}

void ModuleCamera::ViewportResized(int width, int height) {
	for (ComponentCamera& camera : App->scene->scene->cameraComponents) {
		// TODO: Implement button to force AspectRatio from specific camera
		camera.frustum.SetVerticalFovAndAspectRatio(camera.frustum.VerticalFov(), width / (float) height);
	}
	engineCameraFrustum.SetVerticalFovAndAspectRatio(engineCameraFrustum.VerticalFov(), width / (float) height);
}

void ModuleCamera::SetFOV(float hFov) {
	activeFrustum->SetHorizontalFovAndAspectRatio(hFov, activeFrustum->AspectRatio());
}

void ModuleCamera::SetAspectRatio(float aspectRatio) {
	activeFrustum->SetVerticalFovAndAspectRatio(activeFrustum->VerticalFov(), aspectRatio);
}

void ModuleCamera::SetPlaneDistances(float nearPlane, float farPlane) {
	activeFrustum->SetViewPlaneDistances(nearPlane, farPlane);
}

void ModuleCamera::SetPosition(const vec& position) {
	activeFrustum->SetPos(position);
}

void ModuleCamera::SetPosition(float x, float y, float z) {
	activeFrustum->SetPos(vec(x, y, z));
}

void ModuleCamera::SetOrientation(const float3x3& rotationMatrix) {
	activeFrustum->SetFront(rotationMatrix * float3::unitZ);
	activeFrustum->SetUp(rotationMatrix * float3::unitY);
}

vec ModuleCamera::GetFront() const {
	return activeFrustum->Front();
}

vec ModuleCamera::GetUp() const {
	return activeFrustum->Up();
}

vec ModuleCamera::GetWorldRight() const {
	return activeFrustum->WorldRight();
}

vec ModuleCamera::GetPosition() const {
	return activeFrustum->Pos();
}

float3 ModuleCamera::GetOrientation() const {
	return activeFrustum->ViewMatrix().RotatePart().ToEulerXYZ();
}

float ModuleCamera::GetFocusDistance() const {
	return focusDistance;
}

float ModuleCamera::GetNearPlane() const {
	return activeFrustum->NearPlaneDistance();
}

float ModuleCamera::GetFarPlane() const {
	return activeFrustum->FarPlaneDistance();
}

float ModuleCamera::GetFOV() const {
	return activeFrustum->VerticalFov();
}

float ModuleCamera::GetAspectRatio() const {
	return activeFrustum->AspectRatio();
}

float4x4 ModuleCamera::GetProjectionMatrix() const {
	return activeFrustum->ProjectionMatrix();
}

float4x4 ModuleCamera::GetViewMatrix() const {
	return activeFrustum->ViewMatrix();
}

Frustum ModuleCamera::GetEngineFrustum() const {
	return engineCameraFrustum;
}

Frustum* ModuleCamera::GetActiveFrustum() const {
	return activeFrustum;
}

Frustum* ModuleCamera::GetCullingFrustum() const {
	return cullingFrustum;
}

const FrustumPlanes& ModuleCamera::GetFrustumPlanes() const {
	return frustumPlanes;
}

void ModuleCamera::EnableOrtographic() {
	activeFrustum->SetOrthographic(App->renderer->GetViewportSize().x, App->renderer->GetViewportSize().y);
}

void ModuleCamera::EnablePerspective() {
	activeFrustum->SetPerspective(1.3f, 1.f);
	ViewportResized(App->renderer->GetViewportSize().x, App->renderer->GetViewportSize().y);
}

void ModuleCamera::GetIntersectingAABBRecursive(const Quadtree<GameObject>::Node& node, const AABB2D& nodeAABB, const LineSegment& ray, std::vector<GameObject*>& intersectingObjects) {
	AABB nodeAABB3d = AABB({nodeAABB.minPoint.x, -1000000.0f, nodeAABB.minPoint.y}, {nodeAABB.maxPoint.x, 1000000.0f, nodeAABB.maxPoint.y});
	if (ray.Intersects(nodeAABB3d)) {
		if (node.IsBranch()) {
			vec2d center = nodeAABB.minPoint + (nodeAABB.maxPoint - nodeAABB.minPoint) * 0.5f;

			const Quadtree<GameObject>::Node& topLeft = node.childNodes->nodes[0];
			AABB2D topLeftAABB = {{nodeAABB.minPoint.x, center.y}, {center.x, nodeAABB.maxPoint.y}};
			GetIntersectingAABBRecursive(topLeft, topLeftAABB, ray, intersectingObjects);

			const Quadtree<GameObject>::Node& topRight = node.childNodes->nodes[1];
			AABB2D topRightAABB = {{center.x, center.y}, {nodeAABB.maxPoint.x, nodeAABB.maxPoint.y}};
			GetIntersectingAABBRecursive(topRight, topRightAABB, ray, intersectingObjects);

			const Quadtree<GameObject>::Node& bottomLeft = node.childNodes->nodes[2];
			AABB2D bottomLeftAABB = {{nodeAABB.minPoint.x, nodeAABB.minPoint.y}, {center.x, center.y}};
			GetIntersectingAABBRecursive(bottomLeft, bottomLeftAABB, ray, intersectingObjects);

			const Quadtree<GameObject>::Node& bottomRight = node.childNodes->nodes[3];
			AABB2D bottomRightAABB = {{center.x, nodeAABB.minPoint.y}, {nodeAABB.maxPoint.x, center.y}};
			GetIntersectingAABBRecursive(bottomRight, bottomRightAABB, ray, intersectingObjects);
		} else {
			const Quadtree<GameObject>::Element* element = node.firstElement;
			while (element != nullptr) {
				GameObject* gameObject = element->object;
				if (!gameObject->flag) {
					ComponentBoundingBox* boundingBox = gameObject->GetComponent<ComponentBoundingBox>();
					const AABB& gameObjectAABB = boundingBox->GetWorldAABB();
					if (ray.Intersects(gameObjectAABB)) {
						intersectingObjects.push_back(gameObject);
					}

					gameObject->flag = true;
				}
				element = element->next;
			}
		}
	}
}
