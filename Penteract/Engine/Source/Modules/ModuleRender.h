#pragma once

#include "Module.h"
#include "Utils/Quadtree.h"
#include "Rendering/LightFrustum.h"

#include "MathGeoLibFwd.h"
#include "Math/float3.h"

#include <map>

#define SSAO_KERNEL_SIZE 64
#define RANDOM_TANGENTS_ROWS 4
#define RANDOM_TANGENTS_COLS 4

class GameObject;
class ComponentLight;

enum class TESSERACT_ENGINE_API MSAA_SAMPLES_TYPE {
	MSAA_X2,
	MSAA_X4,
	MSAA_X8,
	COUNT
};

struct Light {
	float3 pos = float3::zero;
	int isSpotLight = 0;
	float3 direction = float3::zero;
	float intensity = 0.0f;
	float3 color = float3::zero;
	float radius = 0.0f;
	int useCustomFalloff = 0;
	float falloffExponent = 0.0f;
	float innerAngle = 0.0f;
	float outerAngle = 400.0f;
};

struct LightTile {
	unsigned count = 0;
	unsigned offset = 0;
};

struct TileFrustum {
	float4 planeNormals[4];
};

class ModuleRender : public Module {
public:
	// ------- Core Functions ------ //
	bool Init() override;

	bool Start() override;
	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;
	bool CleanUp() override;
	void ReceiveEvent(TesseractEvent& ev) override;

	void ViewportResized(int width, int height); // Updates the viewport aspect ratio with the new one given by parameters. It will set 'viewportUpdated' to true, to regenerate the framebuffer to its new size using UpdateFramebuffers().
	void UpdateFramebuffers();					 // Generates the rendering framebuffer on Init(). If 'viewportUpdated' was set to true, it will be also called at PostUpdate().
	void ComputeBloomGaussianKernel();
	void ComputeLightTileFrustums();

	void SetVSync(bool vsync);

	// -------- Game Debug --------- //
	void ToggleDebugMode();
	void ToggleDebugDraw();
	void ToggleDrawQuadtree();
	void ToggleDrawBBoxes();
	void ToggleDrawSkybox(); // TODO: review Godmodecamera
	void ToggleDrawAnimationBones();
	void ToggleDrawCameraFrustums();
	void ToggleDrawLightGizmos();
	void ToggleDrawParticleGizmos();

	void UpdateShadingMode(const char* shadingMode);

	float4x4 GetLightViewMatrix(unsigned int i, ShadowCasterType lightFrustumType) const;
	float4x4 GetLightProjectionMatrix(unsigned int i, ShadowCasterType lightFrustumType) const;

	int GetLightTilesPerRow() const;

	int GetCulledTriangles() const;
	const float2 GetViewportSize();

	bool ObjectInsideFrustum(GameObject* gameObject);

public:
	void* context = nullptr; // SDL context.

	// - Render Buffer GL pointers - //
	unsigned cubeVAO = 0;
	unsigned cubeVBO = 0;

	unsigned lightTileFrustumsStorageBuffer = 0;
	unsigned lightsStorageBuffer = 0;
	unsigned lightIndicesCountStorageBufferOpaque = 0;
	unsigned lightIndicesStorageBufferOpaque = 0;
	unsigned lightTilesStorageBufferOpaque = 0;
	unsigned lightIndicesCountStorageBufferTransparent = 0;
	unsigned lightIndicesStorageBufferTransparent = 0;
	unsigned lightTilesStorageBufferTransparent = 0;

	unsigned renderTexture = 0;
	unsigned outputTexture = 0;
	unsigned depthsMSTexture = 0;
	unsigned positionsMSTexture = 0;
	unsigned normalsMSTexture = 0;
	unsigned depthsTexture = 0;
	unsigned positionsTexture = 0;
	unsigned normalsTexture = 0;
	std::vector<unsigned> depthMapStaticTextures;
	std::vector<unsigned> depthMapDynamicTextures;
	std::vector<unsigned> depthMapMainEntitiesTextures;
	unsigned ssaoTexture = 0;
	unsigned auxBlurTexture = 0;
	unsigned colorTextures[2] = {0, 0}; // position 0: scene render texture; position 1: bloom texture to be blurred
	unsigned bloomBlurTextures[2] = {0, 0};
	unsigned bloomCombineTexture = 0;

	unsigned renderPassBuffer = 0;
	unsigned depthPrepassBuffer = 0;
	unsigned depthPrepassTextureConversionBuffer = 0;
	std::vector<unsigned> depthMapStaticTextureBuffers;
	std::vector<unsigned> depthMapDynamicTextureBuffers;
	std::vector<unsigned> depthMapMainEntitiesTextureBuffers;
	unsigned ssaoTextureBuffer = 0;
	unsigned ssaoBlurTextureBufferH = 0;
	unsigned ssaoBlurTextureBufferV = 0;
	unsigned colorCorrectionBuffer = 0;
	unsigned hdrFramebuffer = 0;
	unsigned bloomBlurFramebuffers[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Ping-pong buffers to blur bloom horizontally and vertically
	unsigned bloomCombineFramebuffers[5] = {0, 0, 0, 0, 0};

	// -- Debugging Tools Toggles -- //
	bool debugMode = false; // Flag to activate DrawOptions only ingame (not use in the engine)
	bool drawDebugDraw = true;
	bool drawQuadtree = true;
	bool drawAllBoundingBoxes = false;
	bool skyboxActive = true; // TODO: review Godmodecamera
	bool drawAllBones = false;
	bool drawCameraFrustums = false;
	bool drawLightGizmos = false;
	bool drawStaticLightFrustumOrtographicGizmo = false;
	bool drawStaticLightFrustumPerspectiveGizmo = false;
	bool drawDynamicLightFrustumOrtographicGizmo = false;
	bool drawDynamicLightFrustumPerspectiveGizmo = false;
	bool drawMainEntitiesLightFrustumOrtographicGizmo = false;
	bool drawMainEntitiesLightFrustumPerspectiveGizmo = false;
	bool drawNavMesh = false;
	bool drawParticleGizmos = false;
	bool drawColliders = false;
	int culledTriangles = 0;

	int indexStaticOrtographic = INT_MAX;
	int indexStaticPerspective = INT_MAX;
	int indexDynamicOrtographic = INT_MAX;
	int indexDynamicPerspective = INT_MAX;
	int indexMainEntitiesOrtographic = INT_MAX;
	int indexMainEntitiesPerspective = INT_MAX;

	float3 clearColor = {0.1f, 0.1f, 0.1f};		 // Color of the viewport between frames

	// SSAO
	bool ssaoActive = true;
	float ssaoRange = 1.0f;
	float ssaoBias = 0.0f;
	float ssaoPower = 3.0f;
	float ssaoDirectLightingStrength = 0.5f;

	// Bloom
	bool bloomActive = true;
	int gaussSSAOKernelRadius = 0;
	int gaussBloomKernelRadius = 0;

	int gaussVerySmallMipLevel = 1;
	int gaussSmallMipLevel = 2;
	int gaussMediumMipLevel = 3;
	int gaussLargeMipLevel = 4;
	int gaussVeryLargeMipLevel = 5;

	float bloomIntensity = 1.0f;
	float bloomThreshold = 1.0f;
	float bloomSizeMultiplier = 1.0f;
	float bloomVerySmallWeight = 0.5f;
	float bloomSmallWeight = 0.5f;
	float bloomMediumWeight = 0.5f;
	float bloomLargeWeight = 0.5f;
	float bloomVeryLargeWeight = 0.5f;

	bool msaaActive = true;
	MSAA_SAMPLES_TYPE msaaSampleType = MSAA_SAMPLES_TYPE::MSAA_X4;
	int msaaSamplesNumber[static_cast<int>(MSAA_SAMPLES_TYPE::COUNT)] = {2, 4, 8};
	int msaaSampleSingle = 1;

	bool chromaticAberrationActive = false;
	float chromaticAberrationStrength = 1.0f;

	// Shadows

	float shadowAttenuation = 0.35f;

	LightFrustum lightFrustumStatic;
	LightFrustum lightFrustumDynamic;
	LightFrustum lightFrustumMainEntities;

private:
	void DrawQuadtreeRecursive(const Quadtree<GameObject>::Node& node, const AABB2D& aabb);			  // Draws the quadrtee nodes if 'drawQuadtree' is set to true.
	void ClassifyGameObjects();																		  // Classify Game Objects from Scene taking into account Frustum Culling, Shadows and Rendering Mode
	void ClassifyGameObjectsFromQuadtree(const Quadtree<GameObject>::Node& node, const AABB2D& aabb); // Classify Game Objects from Scene taking into account Frustum Culling, Quadtree, Shadows and Rendering Mode
	void DrawGameObject(GameObject* gameObject);													  // ??
	void DrawGameObjectDepthPrepass(GameObject* gameObject);
	void DrawGameObjectShadowPass(GameObject* gameObject, unsigned int i, ShadowCasterType lightFrustumType);
	void DrawAnimation(const GameObject* gameObject, bool hasAnimation = false);
	void RenderUI();
	void SetOrtographicRender();
	void SetPerspectiveRender();

	void FillLightTiles();

	void ConvertDepthPrepassTextures();
	void ComputeSSAOTexture();
	void BlurSSAOTexture(bool horizontal);
	void BlurBloomTexture(unsigned bloomTexture, bool horizontal, const std::vector<float>& kernel, int kernelRadius, int textureLevel);
	void BloomCombine(unsigned bloomTexture, int bloomTextureLevel, int brightTextureLevel, float bloomWeight);

	void ExecuteColorCorrection();

	void DrawTexture(unsigned texture);
	void DrawLightTiles(bool opaque);
	void DrawScene();

private:
	// ------- Viewport ------- //
	bool viewportUpdated = true;
	float2 viewportSize = float2::zero;
	float2 updatedViewportSize = float2::zero;

	int lightTilesPerRow = 0;
	int lightTilesPerColumn = 0;
	bool lightTilesComputed = false;

	unsigned int indexDepthMapTexture = UINT_MAX;
	ShadowCasterType shadowCasterType;
	bool drawWireframe = false;
	bool drawDepthMapTexture = false;
	bool drawSSAOTexture = false;
	bool drawNormalsTexture = false;
	bool drawPositionsTexture = false;
	bool drawLightTilesOpaque = false;
	bool drawLightTilesTransparent = false;

	std::vector<GameObject*> opaqueGameObjects;			 // Vector of Opaque GameObjects
	std::map<float, GameObject*> transparentGameObjects; // Map with Transparent GameObjects

	// ------- Kernels ------- //
	std::vector<float> ssaoGaussKernel;
	std::vector<float> bloomGaussKernel;

	float3 ssaoKernel[SSAO_KERNEL_SIZE];
	float3 randomTangents[RANDOM_TANGENTS_ROWS * RANDOM_TANGENTS_COLS];
};
