#pragma once

#include "Component.h"

#include "Geometry/Frustum.h"

class ComponentCamera : public Component {
public:
	REGISTER_COMPONENT(ComponentCamera, ComponentType::CAMERA, false); // Refer to ComponentType for the Constructor

	// ------- Core Functions ------ //
	void Init() override;
	void Update() override;
	void DrawGizmos() override;
	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;
	void DuplicateComponent(GameObject& owner) override;

	void UpdateFrustum();
	Frustum BuildDefaultFrustum() const; // Builds a frustum object, at origin, facing Z direction, with specified FOV.

	// ------- Getters ------ //
	TESSERACT_ENGINE_API Frustum* GetFrustum();

public:
	Frustum frustum = BuildDefaultFrustum(); // Frustum geometry of the camera
private:
	bool activeCamera = false;	// Indicator if this is the active camera. The active camera is the POV the scene will be rendered from.
	bool cullingCamera = false; // Indicator if this camera perfoms frustum culling. Meshes outside the frustum will not be rendered when set to true.
};
