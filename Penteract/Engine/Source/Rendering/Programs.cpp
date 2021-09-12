#include "Programs.h"

#include "GL/glew.h"
#include <string>

PointLightUniforms::PointLightUniforms() {}

PointLightUniforms::PointLightUniforms(unsigned program, unsigned number) {
	posLocation = glGetUniformLocation(program, (std::string("light.points[") + std::to_string(number) + "].pos").c_str());
	colorLocation = glGetUniformLocation(program, (std::string("light.points[") + std::to_string(number) + "].color").c_str());
	intensityLocation = glGetUniformLocation(program, (std::string("light.points[") + std::to_string(number) + "].intensity").c_str());
	radiusLocation = glGetUniformLocation(program, (std::string("light.points[") + std::to_string(number) + "].radius").c_str());
	useCustomFalloffLocation = glGetUniformLocation(program, (std::string("light.points[") + std::to_string(number) + "].useCustomFalloff").c_str());
	falloffExponentLocation = glGetUniformLocation(program, (std::string("light.points[") + std::to_string(number) + "].falloffExponent").c_str());
}

SpotLightUniforms::SpotLightUniforms() {}

SpotLightUniforms::SpotLightUniforms(unsigned program, unsigned number) {
	posLocation = glGetUniformLocation(program, (std::string("light.spots[") + std::to_string(number) + "].pos").c_str());
	directionLocation = glGetUniformLocation(program, (std::string("light.spots[") + std::to_string(number) + "].direction").c_str());
	colorLocation = glGetUniformLocation(program, (std::string("light.spots[") + std::to_string(number) + "].color").c_str());
	intensityLocation = glGetUniformLocation(program, (std::string("light.spots[") + std::to_string(number) + "].intensity").c_str());
	radiusLocation = glGetUniformLocation(program, (std::string("light.spots[") + std::to_string(number) + "].radius").c_str());
	useCustomFalloffLocation = glGetUniformLocation(program, (std::string("light.spots[") + std::to_string(number) + "].useCustomFalloff").c_str());
	falloffExponentLocation = glGetUniformLocation(program, (std::string("light.spots[") + std::to_string(number) + "].falloffExponent").c_str());
	innerAngleLocation = glGetUniformLocation(program, (std::string("light.spots[") + std::to_string(number) + "].innerAngle").c_str());
	outerAngleLocation = glGetUniformLocation(program, (std::string("light.spots[") + std::to_string(number) + "].outerAngle").c_str());
}

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

	viewLightLocation = glGetUniformLocation(program, "viewLight");
	projLightLocation = glGetUniformLocation(program, "projLight");

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

	depthMapTextureLocation = glGetUniformLocation(program, "depthMapTexture");

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

	lightAmbientColorLocation = glGetUniformLocation(program, "light.ambient.color");

	lightDirectionalDirectionLocation = glGetUniformLocation(program, "light.directional.direction");
	lightDirectionalColorLocation = glGetUniformLocation(program, "light.directional.color");
	lightDirectionalIntensityLocation = glGetUniformLocation(program, "light.directional.intensity");
	lightDirectionalIsActiveLocation = glGetUniformLocation(program, "light.directional.isActive");

	lightNumPointsLocation = glGetUniformLocation(program, "light.numPoints");
	for (unsigned i = 0; i < POINT_LIGHTS; ++i) {
		lightPoints[i] = PointLightUniforms(program, i);
	}

	lightNumSpotsLocation = glGetUniformLocation(program, "light.numSpots");
	for (unsigned i = 0; i < SPOT_LIGHTS; ++i) {
		lightSpots[i] = SpotLightUniforms(program, i);
	}
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

ProgramStandardDissolve::ProgramStandardDissolve(unsigned program)
	: ProgramStandardMetallic(program) {
	scaleLocation = glGetUniformLocation(program, "dissolveScale");
	thresholdLocation = glGetUniformLocation(program, "dissolveThreshold");
	offsetLocation = glGetUniformLocation(program, "dissolveOffset");
	edgeSizeLocation = glGetUniformLocation(program, "edgeSize");
}

ProgramUnlitDissolve::ProgramUnlitDissolve(unsigned program)
	: ProgramUnlit(program) {
	scaleLocation = glGetUniformLocation(program, "dissolveScale");
	thresholdLocation = glGetUniformLocation(program, "dissolveThreshold");
	offsetLocation = glGetUniformLocation(program, "dissolveOffset");
	edgeSizeLocation = glGetUniformLocation(program, "edgeSize");
}

ProgramDepthPrepassDissolve::ProgramDepthPrepassDissolve(unsigned program)
	: ProgramDepthPrepass(program) {
	scaleLocation = glGetUniformLocation(program, "dissolveScale");
	thresholdLocation = glGetUniformLocation(program, "dissolveThreshold");
	offsetLocation = glGetUniformLocation(program, "dissolveOffset");
}
