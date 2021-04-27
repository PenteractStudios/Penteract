#pragma once

#include "Module.h"
#include "Utils/Quadtree.h"

#include "MathGeoLibFwd.h"
#include "Math/float3.h"

class GameObject;

class ModuleRender : public Module {
public:
	// ------- Core Functions ------ //
	bool Init() override;
	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;
	bool CleanUp() override;

	void ViewportResized(int width, int height);

	void SetVSync(bool vsync);

	// -------- Game Debug --------- //
	TESSERACT_ENGINE_API void ToggleDebugMode();
	TESSERACT_ENGINE_API void ToggleDebugDraw();
	TESSERACT_ENGINE_API void ToggleDrawQuadtree();
	TESSERACT_ENGINE_API void ToggleDrawBBoxes();
	TESSERACT_ENGINE_API void ToggleDrawSkybox(); // TODO: review Godmodecamera
	TESSERACT_ENGINE_API void ToggleDrawAnimationBones();
	TESSERACT_ENGINE_API void ToggleDrawCameraFrustums();
	TESSERACT_ENGINE_API void ToggleDrawLightGizmos();

	void UpdateShadingMode(const char* shadingMode);

	int GetCulledTriangles() const;
	const float2 GetViewportSize();

public:
	void* context = nullptr; // SDL context.

	// - Render Buffer GL pointers - //
	unsigned renderTexture = 0;
	unsigned depthRenderbuffer = 0;
	unsigned framebuffer = 0;

	// -- Debugging Tools Toggles -- //
	bool debugMode = false; // Flag to activate DrawOptions only ingame (not use in the engine)
	bool drawDebugDraw = true;
	bool drawQuadtree = true;
	bool drawAllBoundingBoxes = false;
	bool skyboxActive = true; // TODO: review Godmodecamera
	bool drawAllBones = true;
	bool drawCameraFrustums = false;
	bool drawLightGizmos = false;
	int culledTriangles = 0;

	float3 ambientColor = {0.0f, 0.0f, 0.0f}; // Color of ambient Light
	float3 clearColor = {0.1f, 0.1f, 0.1f};	  // Color of the viewport between frames

private:
	void DrawQuadtreeRecursive(const Quadtree<GameObject>::Node& node, const AABB2D& aabb); // Draws the quadrtee nodes if 'drawQuadtree' is set to true.
	void DrawSceneRecursive(const Quadtree<GameObject>::Node& node, const AABB2D& aabb);	// ??
	bool CheckIfInsideFrustum(const AABB& aabb, const OBB& obb);							// ??
	void DrawGameObject(GameObject* gameObject);											// ??
	void DrawSkyBox();																		// Draws a default skybox if 'skyboxActive' is set to true.
	void DrawAnimation(const GameObject* gameObject, bool hasAnimation = false);
	void RenderUI();
	void SetOrtographicRender();
	void SetPerspectiveRender();

private:
	// ------- Viewport Size ------- //
	float2 viewportSize = float2::zero;
};
