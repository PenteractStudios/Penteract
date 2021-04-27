#pragma once

#include "Component.h"

#include "Math/float3.h"

enum class LightType {
	DIRECTIONAL,
	POINT,
	SPOT
};

class ComponentLight : public Component {
public:
	REGISTER_COMPONENT(ComponentLight, ComponentType::LIGHT, false); // Refer to ComponentType for the Constructor

	// ------- Core Functions ------ //
	void Init() override;
	void Update() override;
	void DrawGizmos() override;
	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;
	void DuplicateComponent(GameObject& owner) override;

	void UpdateLight();

public:
	LightType lightType = LightType::DIRECTIONAL; // Defines the type of the light from one in the LightType enum.
	float3 pos = {0.0f, 0.0f, 0.0f};			  // World coordinates from where the light is emitting. Corresponds to the GameObject Transform and will update on OnTransformUpdate().
	float3 direction = {0.0f, 0.0f, 0.0f};		  // Direction the light emits to. Corresponds to the GameObject Rotation and will update on OnTransformUpdate().
	float3 color = {1.0f, 1.0f, 1.0f};			  // RGB color that will be reflected in the objects.

	// -------- Attenuation -------- //
	// Inital default values to rise 100 meters.
	float intensity = 1.0f;
	float kc = 1.0f;		//Keep in one to avoid having denominator less than 1
	float kl = 0.045f;
	float kq = 0.0075f;
	float innerAngle = pi / 12;
	float outerAngle = pi / 6;
};
