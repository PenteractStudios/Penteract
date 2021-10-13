#include "Programs.h"

#include "GL/glew.h"
#include <string>

Program::Program(unsigned program_)
	: program(program_) {}

Program::~Program() {
	glDeleteProgram(program);
}

ProgramCubemapRender::ProgramCubemapRender(unsigned program_)
	: Program(program_) {
	viewLocation = glGetUniformLocation(program, "view");
	projLocation = glGetUniformLocation(program, "proj");
}

ProgramHDRToCubemap::ProgramHDRToCubemap(unsigned program_)
	: ProgramCubemapRender(program_) {
	hdrLocation = glGetUniformLocation(program, "hdr");
}

ProgramIrradiance::ProgramIrradiance(unsigned program_)
	: ProgramCubemapRender(program_) {
	environmentLocation = glGetUniformLocation(program, "environment");
}

ProgramPreFilteredMap::ProgramPreFilteredMap(unsigned program_)
	: ProgramCubemapRender(program_) {
	environmentLocation = glGetUniformLocation(program, "environment");
	environmentResolutionLocation = glGetUniformLocation(program, "environmentResolution");
	roughnessLocation = glGetUniformLocation(program, "roughness");
}

ProgramEnvironmentBRDF::ProgramEnvironmentBRDF(unsigned program_)
	: Program(program_) {
}

ProgramSkybox::ProgramSkybox(unsigned program_)
	: Program(program_) {
	viewLocation = glGetUniformLocation(program, "view");
	projLocation = glGetUniformLocation(program, "proj");

	cubemapLocation = glGetUniformLocation(program, "cubemap");
}

ProgramGridFrustumsCompute::ProgramGridFrustumsCompute(unsigned program_)
	: Program(program_) {
	invProjLocation = glGetUniformLocation(program, "invProj");

	screenSizeLocation = glGetUniformLocation(program, "screenSize");
	numThreadsLocation = glGetUniformLocation(program, "numThreads");
}

ProgramLightCullingCompute::ProgramLightCullingCompute(unsigned program_)
	: Program(program_) {
	invProjLocation = glGetUniformLocation(program, "invProj");
	viewLocation = glGetUniformLocation(program, "view");

	screenSizeLocation = glGetUniformLocation(program, "screenSize");
	lightCountLocation = glGetUniformLocation(program, "lightCount");

	depthsLocation = glGetUniformLocation(program, "depths");
}

ProgramUnlit::ProgramUnlit(unsigned program_)
	: Program(program_) {
	modelLocation = glGetUniformLocation(program, "model");
	viewLocation = glGetUniformLocation(program, "view");
	projLocation = glGetUniformLocation(program, "proj");

	paletteLocation = glGetUniformLocation(program, "palette");
	hasBonesLocation = glGetUniformLocation(program, "hasBones");

	diffuseMapLocation = glGetUniformLocation(program, "diffuseMap");
	diffuseColorLocation = glGetUniformLocation(program, "diffuseColor");
	hasDiffuseMapLocation = glGetUniformLocation(program, "hasDiffuseMap");

	emissiveMapLocation = glGetUniformLocation(program, "emissiveMap");
	hasEmissiveMapLocation = glGetUniformLocation(program, "hasEmissiveMap");
	emissiveIntensityLocation = glGetUniformLocation(program, "emissiveIntensity");
	emissiveColorLocation = glGetUniformLocation(program, "emissiveColor");

	tilingLocation = glGetUniformLocation(program, "tiling");
	offsetLocation = glGetUniformLocation(program, "offset");
}

ProgramVolumetricLight::ProgramVolumetricLight(unsigned program_)
	: Program(program_) {
	modelLocation = glGetUniformLocation(program, "model");
	viewLocation = glGetUniformLocation(program, "view");
	projLocation = glGetUniformLocation(program, "proj");

	paletteLocation = glGetUniformLocation(program, "palette");
	hasBonesLocation = glGetUniformLocation(program, "hasBones");

	viewPosLocation = glGetUniformLocation(program, "viewPos");

	nearLocation = glGetUniformLocation(program, "near");
	farLocation = glGetUniformLocation(program, "far");

	depthsLocation = glGetUniformLocation(program, "depths");

	lightColorLocation = glGetUniformLocation(program, "lightColor");
	lightMapLocation = glGetUniformLocation(program, "lightMap");
	hasLightMapLocation = glGetUniformLocation(program, "hasLightMap");
	intensityLocation = glGetUniformLocation(program, "intensity");
	attenuationExponentLocation = glGetUniformLocation(program, "attenuationExponent");

	isSoftLocation = glGetUniformLocation(program, "isSoft");
	softRangeLocation = glGetUniformLocation(program, "softRange");
}

ProgramStandard::ProgramStandard(unsigned program_)
	: Program(program_) {
	modelLocation = glGetUniformLocation(program, "model");
	viewLocation = glGetUniformLocation(program, "view");
	projLocation = glGetUniformLocation(program, "proj");

	isOpaqueLocation = glGetUniformLocation(program, "isOpaque");

	viewOrtoLightsStaticLocation = glGetUniformLocation(program, "viewOrtoLightsStatic");
	projOrtoLightsStaticLocation = glGetUniformLocation(program, "projOrtoLightsStatic");
	
	viewOrtoLightsDynamicLocation = glGetUniformLocation(program, "viewOrtoLightsDynamic");
	projOrtoLightsDynamicLocation = glGetUniformLocation(program, "projOrtoLightsDynamic");

	shadowCascadesCounterLocation = glGetUniformLocation(program, "shadowCascadesCounter");

	for (unsigned int i = 0; i < CASCADE_FRUSTUMS; ++i) {
		depthMaps[i] = DepthMapsUniforms(program, i);
	}

	paletteLocation = glGetUniformLocation(program, "palette");
	hasBonesLocation = glGetUniformLocation(program, "hasBones");

	viewPosLocation = glGetUniformLocation(program, "viewPos");

	diffuseMapLocation = glGetUniformLocation(program, "diffuseMap");
	diffuseColorLocation = glGetUniformLocation(program, "diffuseColor");
	hasDiffuseMapLocation = glGetUniformLocation(program, "hasDiffuseMap");
	smoothnessLocation = glGetUniformLocation(program, "smoothness");
	hasSmoothnessAlphaLocation = glGetUniformLocation(program, "hasSmoothnessAlpha");

	normalMapLocation = glGetUniformLocation(program, "normalMap");
	hasNormalMapLocation = glGetUniformLocation(program, "hasNormalMap");
	normalStrengthLocation = glGetUniformLocation(program, "normalStrength");

	emissiveMapLocation = glGetUniformLocation(program, "emissiveMap");
	hasEmissiveMapLocation = glGetUniformLocation(program, "hasEmissiveMap");
	emissiveColorLocation = glGetUniformLocation(program, "emissiveColor");
	emissiveIntensityLocation = glGetUniformLocation(program, "emissiveIntensity");

	ambientOcclusionMapLocation = glGetUniformLocation(program, "ambientOcclusionMap");
	hasAmbientOcclusionMapLocation = glGetUniformLocation(program, "hasAmbientOcclusionMap");

	ssaoTextureLocation = glGetUniformLocation(program, "ssaoTexture");
	ssaoDirectLightingStrengthLocation = glGetUniformLocation(program, "ssaoDirectLightingStrength");

	tilingLocation = glGetUniformLocation(program, "tiling");
	offsetLocation = glGetUniformLocation(program, "offset");

	hasIBLLocation = glGetUniformLocation(program, "hasIBL");
	diffuseIBLLocation = glGetUniformLocation(program, "diffuseIBL");
	prefilteredIBLLocation = glGetUniformLocation(program, "prefilteredIBL");
	environmentBRDFLocation = glGetUniformLocation(program, "environmentBRDF");
	prefilteredIBLNumLevelsLocation = glGetUniformLocation(program, "prefilteredIBLNumLevels");
	strengthIBLLocation = glGetUniformLocation(program, "strengthIBL");

	ambientColorLocation = glGetUniformLocation(program, "ambientColor");

	dirLightDirectionLocation = glGetUniformLocation(program, "dirLight.direction");
	dirLightColorLocation = glGetUniformLocation(program, "dirLight.color");
	dirLightIntensityLocation = glGetUniformLocation(program, "dirLight.intensity");
	dirLightIsActiveLocation = glGetUniformLocation(program, "dirLight.isActive");

	tilesPerRowLocation = glGetUniformLocation(program, "tilesPerRow");
}

ProgramStandardPhong::ProgramStandardPhong(unsigned program_)
	: ProgramStandard(program_) {
	specularColorLocation = glGetUniformLocation(program, "specularColor");
	hasSpecularMapLocation = glGetUniformLocation(program, "hasSpecularMap");
	specularMapLocation = glGetUniformLocation(program, "specularMap");
}

ProgramStandardSpecular::ProgramStandardSpecular(unsigned program_)
	: ProgramStandard(program_) {
	specularColorLocation = glGetUniformLocation(program, "specularColor");
	hasSpecularMapLocation = glGetUniformLocation(program, "hasSpecularMap");
	specularMapLocation = glGetUniformLocation(program, "specularMap");
}

ProgramStandardMetallic::ProgramStandardMetallic(unsigned program_)
	: ProgramStandard(program_) {
	metalnessLocation = glGetUniformLocation(program, "metalness");
	hasMetallicMapLocation = glGetUniformLocation(program, "hasMetallicMap");
	metallicMapLocation = glGetUniformLocation(program, "metallicMap");
}

ProgramDepthPrepass::ProgramDepthPrepass(unsigned program_)
	: Program(program_) {
	modelLocation = glGetUniformLocation(program, "model");
	viewLocation = glGetUniformLocation(program, "view");
	projLocation = glGetUniformLocation(program, "proj");

	diffuseMapLocation = glGetUniformLocation(program, "diffuseMap");
	diffuseColorLocation = glGetUniformLocation(program, "diffuseColor");
	hasDiffuseMapLocation = glGetUniformLocation(program, "hasDiffuseMap");

	paletteLocation = glGetUniformLocation(program, "palette");
	hasBonesLocation = glGetUniformLocation(program, "hasBones");

	tilingLocation = glGetUniformLocation(program, "tiling");
	offsetLocation = glGetUniformLocation(program, "offset");
}

ProgramDepthPrepassConvertTextures::ProgramDepthPrepassConvertTextures(unsigned program_)
	: Program(program_) {
	samplesNumberLocation = glGetUniformLocation(program, "samplesNumber");

	depthsLocation = glGetUniformLocation(program, "depths");
	positionsLocation = glGetUniformLocation(program, "positions");
	normalsLocation = glGetUniformLocation(program, "normals");
}

ProgramSSAO::ProgramSSAO(unsigned program_)
	: Program(program_) {
	projLocation = glGetUniformLocation(program, "proj");

	positionsLocation = glGetUniformLocation(program, "positions");
	normalsLocation = glGetUniformLocation(program, "normals");

	kernelSamplesLocation = glGetUniformLocation(program, "kernelSamples");
	randomTangentsLocation = glGetUniformLocation(program, "randomTangents");
	screenSizeLocation = glGetUniformLocation(program, "screenSize");
	biasLocation = glGetUniformLocation(program, "bias");
	rangeLocation = glGetUniformLocation(program, "range");
	powerLocation = glGetUniformLocation(program, "power");
}

ProgramBlur::ProgramBlur(unsigned program_)
	: Program(program_) {
	inputTextureLocation = glGetUniformLocation(program, "inputTexture");
	textureLevelLocation = glGetUniformLocation(program, "textureLevel");

	kernelLocation = glGetUniformLocation(program, "kernel");
	kernelRadiusLocation = glGetUniformLocation(program, "kernelRadius");
	horizontalLocation = glGetUniformLocation(program, "horizontal");
}

ProgramBloomCombine::ProgramBloomCombine(unsigned program_)
	: Program(program_) {
	brightTextureLocation = glGetUniformLocation(program, "brightTexture");
	bloomTextureLocation = glGetUniformLocation(program, "bloomTexture");
	brightLevelLocation = glGetUniformLocation(program, "brightLevel");
	bloomLevelLocation = glGetUniformLocation(program, "bloomLevel");
	bloomWeightLocation = glGetUniformLocation(program, "bloomWeight");
}

ProgramPostprocess::ProgramPostprocess(unsigned program_)
	: Program(program_) {
	sceneTextureLocation = glGetUniformLocation(program, "sceneTexture");
	bloomThresholdLocation = glGetUniformLocation(program, "bloomThreshold");
	samplesNumberLocation = glGetUniformLocation(program, "samplesNumber");
	bloomActiveLocation = glGetUniformLocation(program, "bloomActive");
}

ProgramColorCorrection::ProgramColorCorrection(unsigned program_)
	: Program(program_) {
	sceneTextureLocation = glGetUniformLocation(program, "sceneTexture");
	bloomTextureLocation = glGetUniformLocation(program, "bloomTexture");
	hasBloomLocation = glGetUniformLocation(program, "hasBloom");
	bloomIntensityLocation = glGetUniformLocation(program, "bloomIntensity");

	hasChromaticAberrationLocation = glGetUniformLocation(program, "hasChromaticAberration");
	chromaticAberrationStrengthLocation = glGetUniformLocation(program, "chromaticAberrationStrength");
}

ProgramHeightFog::ProgramHeightFog(unsigned program_)
	: Program(program_) {
	viewLocation = glGetUniformLocation(program, "view");
	projLocation = glGetUniformLocation(program, "proj");

	positionsLocation = glGetUniformLocation(program, "positions");

	viewPosLocation = glGetUniformLocation(program, "viewPos");

	densityLocation = glGetUniformLocation(program, "density");
	falloffLocation = glGetUniformLocation(program, "falloff");
	heightLocation = glGetUniformLocation(program, "height");
	inscatteringColorLocation = glGetUniformLocation(program, "inscatteringColor");
}

ProgramDrawTexture::ProgramDrawTexture(unsigned program_)
	: Program(program_) {
	textureToDrawLocation = glGetUniformLocation(program, "textureToDraw");
}

ProgramDrawLightTiles::ProgramDrawLightTiles(unsigned program_)
	: Program(program_) {
	tilesPerRowLocation = glGetUniformLocation(program, "tilesPerRow");
}

ProgramImageUI::ProgramImageUI(unsigned program_)
	: Program(program_) {
	modelLocation = glGetUniformLocation(program, "model");
	viewLocation = glGetUniformLocation(program, "view");
	projLocation = glGetUniformLocation(program, "proj");

	inputColorLocation = glGetUniformLocation(program, "inputColor");
	hasDiffuseLocation = glGetUniformLocation(program, "hasDiffuse");
	diffuseLocation = glGetUniformLocation(program, "diffuse");
	offsetLocation = glGetUniformLocation(program, "offset");
	tilingLocation = glGetUniformLocation(program, "tiling");
}

ProgramTextUI::ProgramTextUI(unsigned program_)
	: Program(program_) {
	modelLocation = glGetUniformLocation(program, "model");
	viewLocation = glGetUniformLocation(program, "view");
	projLocation = glGetUniformLocation(program, "proj");

	textColorLocation = glGetUniformLocation(program, "textColor");
}

ProgramBillboard::ProgramBillboard(unsigned program_)
	: Program(program_) {
	modelLocation = glGetUniformLocation(program, "model");
	viewLocation = glGetUniformLocation(program, "view");
	projLocation = glGetUniformLocation(program, "proj");

	nearLocation = glGetUniformLocation(program, "near");
	farLocation = glGetUniformLocation(program, "far");

	transparentLocation = glGetUniformLocation(program, "transparent");

	depthsLocation = glGetUniformLocation(program, "depths");

	diffuseMapLocation = glGetUniformLocation(program, "diffuseMap");
	hasDiffuseLocation = glGetUniformLocation(program, "hasDiffuseMap");
	inputColorLocation = glGetUniformLocation(program, "inputColor");
	intensityLocation = glGetUniformLocation(program, "intensity");

	currentFrameLocation = glGetUniformLocation(program, "currentFrame");
	xTilesLocation = glGetUniformLocation(program, "Xtiles");
	yTilesLocation = glGetUniformLocation(program, "Ytiles");
	xFlipLocation = glGetUniformLocation(program, "flipX");
	yFlipLocation = glGetUniformLocation(program, "flipY");

	isSoftLocation = glGetUniformLocation(program, "isSoft");
	softRangeLocation = glGetUniformLocation(program, "softRange");
}

ProgramTrail::ProgramTrail(unsigned program_)
	: Program(program_) {
	viewLocation = glGetUniformLocation(program, "view");
	projLocation = glGetUniformLocation(program, "proj");
	modelLocation = glGetUniformLocation(program, "model");

	inputColorLocation = glGetUniformLocation(program, "inputColor");
	hasDiffuseLocation = glGetUniformLocation(program, "hasDiffuse");
	diffuseMap = glGetUniformLocation(program, "diffuseMap");

	xFlipLocation = glGetUniformLocation(program, "flipX");
	yFlipLocation = glGetUniformLocation(program, "flipY");
}

DepthMapsUniforms::DepthMapsUniforms() {}

DepthMapsUniforms::DepthMapsUniforms(unsigned program, unsigned number) {
	depthMapLocationStatic = glGetUniformLocation(program, (std::string("depthMapTexturesStatic[") + std::to_string(number) + "]").c_str());
	farPlaneLocationStatic = glGetUniformLocation(program, (std::string("farPlaneDistancesStatic[") + std::to_string(number) + "]").c_str());

	depthMapLocationDynamic = glGetUniformLocation(program, (std::string("depthMapTexturesDynamic[") + std::to_string(number) + "]").c_str());
	farPlaneLocationDynamic = glGetUniformLocation(program, (std::string("farPlaneDistancesDynamic[") + std::to_string(number) + "]").c_str());
}

ProgramStandardDissolve::ProgramStandardDissolve(unsigned program)
	: ProgramStandardMetallic(program) {
	hasNoiseMapLocation = glGetUniformLocation(program, "hasDissolveNoiseMap");
	noiseMapLocation = glGetUniformLocation(program, "dissolveNoiseMap");
	colorLocation = glGetUniformLocation(program, "dissolveColor");
	intensityLocation = glGetUniformLocation(program, "dissolveIntensity");
	scaleLocation = glGetUniformLocation(program, "dissolveScale");
	thresholdLocation = glGetUniformLocation(program, "dissolveThreshold");
	offsetLocation = glGetUniformLocation(program, "dissolveOffset");
	edgeSizeLocation = glGetUniformLocation(program, "edgeSize");
}

ProgramUnlitDissolve::ProgramUnlitDissolve(unsigned program)
	: ProgramUnlit(program) {
	hasNoiseMapLocation = glGetUniformLocation(program, "hasDissolveNoiseMap");
	noiseMapLocation = glGetUniformLocation(program, "dissolveNoiseMap");
	colorLocation = glGetUniformLocation(program, "dissolveColor");
	intensityLocation = glGetUniformLocation(program, "dissolveIntensity");
	scaleLocation = glGetUniformLocation(program, "dissolveScale");
	thresholdLocation = glGetUniformLocation(program, "dissolveThreshold");
	offsetLocation = glGetUniformLocation(program, "dissolveOffset");
	edgeSizeLocation = glGetUniformLocation(program, "edgeSize");
}

ProgramDepthPrepassDissolve::ProgramDepthPrepassDissolve(unsigned program)
	: ProgramDepthPrepass(program) {
	hasNoiseMapLocation = glGetUniformLocation(program, "hasDissolveNoiseMap");
	noiseMapLocation = glGetUniformLocation(program, "dissolveNoiseMap");
	scaleLocation = glGetUniformLocation(program, "dissolveScale");
	thresholdLocation = glGetUniformLocation(program, "dissolveThreshold");
	offsetLocation = glGetUniformLocation(program, "dissolveOffset");
}
