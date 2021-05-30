#include "SpawnOnClick.h"

#include "Resources/ResourcePrefab.h"
#include "GameObject.h"
#include "GameplaySystems.h"

#include "Geometry/LineSegment.h"
#include "Geometry/Plane.h"

EXPOSE_MEMBERS(SpawnOnClick) {
	MEMBER(MemberType::GAME_OBJECT_UID, cameraUID),
	MEMBER(MemberType::PREFAB_RESOURCE_UID, prefabId),
	MEMBER(MemberType::GAME_OBJECT_UID, enemiesUID)
};

GENERATE_BODY_IMPL(SpawnOnClick);

void SpawnOnClick::Start() {
	gameObject = &GetOwner();
	camera = GameplaySystems::GetGameObject(cameraUID);
	enemies = GameplaySystems::GetGameObject(enemiesUID);
}

void SpawnOnClick::Update() {
	if (Input::GetKeyCode(Input::KEYCODE::KEY_LCTRL) && Input::GetMouseButtonUp(0)) {
		ResourcePrefab* prefab = GameplaySystems::GetResource<ResourcePrefab>(prefabId);
		if (prefab != nullptr) {
			UID prefabId = prefab->BuildPrefab(enemies);
			GameObject* go = GameplaySystems::GetGameObject(prefabId);
			go->GetComponent<ComponentTransform>()->SetPosition(DetectMouseLocation());
		}
	}
}

float3 SpawnOnClick::DetectMouseLocation() {
	float2 mousePos = Input::GetMousePositionNormalized();
	ComponentTransform* compTransform = enemies->GetComponent<ComponentTransform>();
	if (camera && compTransform) {
		ComponentCamera* compCamera = camera->GetComponent<ComponentCamera>();
		LineSegment ray = compCamera->frustum.UnProjectLineSegment(mousePos.x, mousePos.y);

		Plane p = Plane(compTransform->GetGlobalPosition(), float3(0, 1, 0));

		return p.ClosestPoint(ray);
	}
}