#include "ComponentMeshRenderer.h"

#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "Utils/ImGuiUtils.h"
#include "Utils/FileDialog.h"
#include "FileSystem/TextureImporter.h"
#include "GameObject.h"
#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceMesh.h"
#include "Resources/ResourceShader.h"
#include "Resources/ResourceTexture.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentLight.h"
#include "Components/ComponentBoundingBox.h"
#include "Components/ComponentAnimation.h"
#include "Modules/ModulePrograms.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleTime.h"

#include "assimp/mesh.h"
#include "GL/glew.h"
#include "imgui_internal.h"
#include "imgui.h"

#include "Utils/Leaks.h"

#define JSON_TAG_MESH_ID "MeshID"
#define JSON_TAG_MATERIAL_ID "MaterialID"

// TODO: Move to Material Importer class

#define JSON_TAG_MATERIAL "Material"
#define JSON_TAG_HAS_DIFFUSE_MAP "HasDiffuseMap"
#define JSON_TAG_DIFFUSE_COLOR "DiffuseColor"
#define JSON_TAG_DIFFUSE_MAP_FILE_NAME "DiffuseMapFileName"
#define JSON_TAG_HAS_SPECULAR_MAP "HasSpecularMap"
#define JSON_TAG_SPECULAR_COLOR "SpecularColor"
#define JSON_TAG_HAS_SPECULAR_MAP_FILE_NAME "SpecularMapFileName"
#define JSON_TAG_SHININESS "Shininess"
#define JSON_TAG_HAS_SHININESS_IN_ALPHA_CHANNEL "HasShininessInAlphaChannel"
#define JSON_TAG_SMOOTHNESS "Smoothness"
#define JSON_TAG_HAS_SMOOTHNESS_IN_ALPHA_CHANNEL "HasSmoothnessInAlphaChannel"
#define JSON_TAG_METALLIC_MAP_FILE_NAME "MetallicMapFileName"
#define JSON_TAG_METALLIC "Metallic"

void ComponentMeshRenderer::OnEditorUpdate() {
	bool active = IsActive();
	if (ImGui::Checkbox("Active", &active)) {
		active ? Enable() : Disable();
	}
	ImGui::Separator();

	UID oldMeshId = meshId;
	ImGui::ResourceSlot<ResourceMesh>("Mesh", &meshId);
	if (oldMeshId != meshId) {
		ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
		transform->InvalidateHierarchy();
	}
	ImGui::ResourceSlot<ResourceMaterial>("Material", &materialId);

	if (ImGui::TreeNode("Mesh")) {
		ResourceMesh* mesh = App->resources->GetResource<ResourceMesh>(meshId);
		if (mesh != nullptr) {
			ImGui::TextColored(App->editor->titleColor, "Geometry");
			ImGui::TextWrapped("Num Vertices: ");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->textColor, "%d", mesh->numVertices);
			ImGui::TextWrapped("Num Triangles: ");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->textColor, "%d", mesh->numIndices / 3);
			ImGui::Separator();
			ImGui::TextColored(App->editor->titleColor, "Bounding Box");

			ImGui::Checkbox("Draw", &bbActive);
			if (bbActive) {
				ComponentBoundingBox* boundingBox = GetOwner().GetComponent<ComponentBoundingBox>();
				boundingBox->DrawBoundingBox();
			}
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Material")) {
		ResourceMaterial* material = App->resources->GetResource<ResourceMaterial>(materialId);
		if (material != nullptr) {
			if (ImGui::Button("Save")) {
				std::string assetPath = material->GetAssetFilePath();
				if (FileDialog::GetFileExtension(assetPath.c_str()) == MATERIAL_EXTENSION) {
					App->resources->assetsToNotUpdate.emplace(assetPath);
					material->SaveToFile(assetPath.c_str());
					material->SaveToFile(material->GetResourceFilePath().c_str());
				} else {
					material->SaveToFile(material->GetResourceFilePath().c_str());
				}
			}
			ImGui::ResourceSlot<ResourceShader>("Shader", &material->shaderId);

			ResourceShader* shader = App->resources->GetResource<ResourceShader>(material->shaderId);
			if (shader != nullptr) {
				ImGui::TextColored(App->editor->titleColor, "Shader");
				ImGui::TextUnformatted("Change type:");
				ImGui::SameLine();
				// Shader types
				const char* shaderTypes[] = {"[Legacy] Phong", "Standard (specular settings)", "Standard"};
				const char* shaderTypesCurrent = shaderTypes[(int) shader->GetShaderType()];
				if (ImGui::BeginCombo("Type", shaderTypesCurrent)) {
					for (int n = 0; n < IM_ARRAYSIZE(shaderTypes); ++n) {
						bool isSelected = (shaderTypesCurrent == shaderTypes[n]);
						if (ImGui::Selectable(shaderTypes[n], isSelected)) {
							shader->SetShaderType(ShaderType(n));
						}
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
					ImGui::Text("");
				}
				if (ImGui::Button("Apply##type")) {
					shader->SaveShaderType();
				}
				if (shader->GetShaderType() == ShaderType::PHONG) {
					ImGui::BeginColumns("##material", 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
					//First Column with the text names of widgets
					{
						//Diffuse
						{
							// TODO: Hint image of texture
							ImGui::ResourceSlot<ResourceTexture>("Diffuse Texture", &material->diffuseMapId);
							if (ImGui::Button("No map##diffuse")) {
								if (material->diffuseMapId != 0) {
									App->resources->DecreaseReferenceCount(material->diffuseMapId);
									material->diffuseMapId = 0;
								}
							}
						}
						//Specular
						{
							ImGui::ResourceSlot<ResourceTexture>("Specular Texture", &material->specularMapId);
							if (ImGui::Button("No map##specular")) {
								if (material->specularMapId != 0) {
									App->resources->DecreaseReferenceCount(material->specularMapId);
									material->specularMapId = 0;
								}
							}
						}
					}

					ImGui::NextColumn();
					//Second column with the widgets
					{
						std::string id_cd("##color_d");
						ImGui::PushID(id_cd.c_str());
						ImGui::ColorEdit4("", material->diffuseColor.ptr(), ImGuiColorEditFlags_NoInputs);
						ImGui::PopID();

						ImGui::NewLine();
						ImGui::NewLine();
						ImGui::NewLine();

						if (material->specularMapId == 0) {
							std::string id_cs("##color_s");
							ImGui::PushID(id_cs.c_str());
							ImGui::ColorEdit4("", material->specularColor.ptr(), ImGuiColorEditFlags_NoInputs);
							ImGui::PopID();
						} else {
							ImGui::NewLine();
						}
					}
					ImGui::EndColumns();
					ImGui::BeginColumns("##material", 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
					{
						// Shininess Combo
						const char* shininessItems[] = {"Shininess Value", "Shininess Alpha"};
						const char* shininessItemCurrent = shininessItems[material->hasSmoothnessInAlphaChannel];
						if (ImGui::BeginCombo("##shininess", shininessItemCurrent)) {
							for (int n = 0; n < IM_ARRAYSIZE(shininessItems); ++n) {
								bool isSelected = (shininessItemCurrent == shininessItems[n]);
								if (ImGui::Selectable(shininessItems[n], isSelected)) {
									material->hasSmoothnessInAlphaChannel = n ? 1 : 0;
								}
								if (isSelected) {
									ImGui::SetItemDefaultFocus();
								}
							}
							ImGui::EndCombo();
						}
					}
					ImGui::NextColumn();
					{
						if (!material->hasSmoothnessInAlphaChannel) {
							std::string id_s("##shininess_");
							ImGui::PushID(id_s.c_str());
							ImGui::DragFloat(id_s.c_str(), &material->smoothness, App->editor->dragSpeed1f, 0, 1000);
							ImGui::PopID();
						}
					}
					ImGui::EndColumns();

				} else if (shader->GetShaderType() == ShaderType::STANDARD_SPECULAR) {
					ImGui::BeginColumns("##material", 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
					//First Column with the text names of widgets
					{
						//Diffuse
						{
							// TODO: Hint image of texture
							ImGui::ResourceSlot<ResourceTexture>("Diffuse Texture", &material->diffuseMapId);
							if (ImGui::Button("No map##diffuse")) {
								if (material->diffuseMapId != 0) {
									App->resources->DecreaseReferenceCount(material->diffuseMapId);
									material->diffuseMapId = 0;
								}
							}
						}
						//Specular
						{
							ImGui::ResourceSlot<ResourceTexture>("Specular Texture", &material->specularMapId);
							if (ImGui::Button("No map##specular")) {
								if (material->specularMapId != 0) {
									App->resources->DecreaseReferenceCount(material->specularMapId);
									material->specularMapId = 0;
								}
							}
						}
					}

					ImGui::NextColumn();
					//Second column with the widgets
					{
						std::string id_cd("##color_d");
						ImGui::PushID(id_cd.c_str());
						ImGui::ColorEdit4("", material->diffuseColor.ptr(), ImGuiColorEditFlags_NoInputs);
						ImGui::PopID();

						ImGui::NewLine();
						ImGui::NewLine();
						ImGui::NewLine();

						if (material->specularMapId == 0) {
							std::string id_cs("##color_s");
							ImGui::PushID(id_cs.c_str());
							ImGui::ColorEdit4("", material->specularColor.ptr(), ImGuiColorEditFlags_NoInputs);
							ImGui::PopID();
						} else {
							ImGui::NewLine();
						}
					}
					ImGui::EndColumns();
					ImGui::BeginColumns("##material", 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
					{
						ImGui::Text("Smoothness");
						// Smoothness combo
						const char* smoothnessItems[] = {"Diffuse Alpha", "Specular Alpha"};
						const char* smoothnessItemCurrent = smoothnessItems[material->hasSmoothnessInAlphaChannel];
						if (ImGui::BeginCombo("##smoothness", smoothnessItemCurrent)) {
							for (int n = 0; n < IM_ARRAYSIZE(smoothnessItems); ++n) {
								bool isSelected = (smoothnessItemCurrent == smoothnessItems[n]);
								if (ImGui::Selectable(smoothnessItems[n], isSelected)) {
									material->hasSmoothnessInAlphaChannel = n;
								}
								if (isSelected) {
									ImGui::SetItemDefaultFocus();
								}
							}
							ImGui::EndCombo();
						}
					}
					ImGui::NextColumn();
					{
						ImGui::NewLine();
						std::string id_s("##smooth_");
						ImGui::PushID(id_s.c_str());
						ImGui::SliderFloat(id_s.c_str(), &material->smoothness, 0, 1);
						ImGui::PopID();
					}
					ImGui::EndColumns();
					// TODO: Normal map

				} else if (shader->GetShaderType() == ShaderType::STANDARD) {
					ImGui::BeginColumns("##material", 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
					//First Column with the text names of widgets
					{
						//Diffuse
						{
							// TODO: Hint image of texture
							ImGui::ResourceSlot<ResourceTexture>("Diffuse Texture", &material->diffuseMapId);
							if (ImGui::Button("No map##diffuse")) {
								if (material->diffuseMapId != 0) {
									App->resources->DecreaseReferenceCount(material->diffuseMapId);
									material->diffuseMapId = 0;
								}
							}
						}
						// Metallic
						{
							ImGui::ResourceSlot<ResourceTexture>("Metallic Texture", &material->metallicMapId);
							if (ImGui::Button("No map##metallic")) {
								if (material->metallicMapId != 0) {
									App->resources->DecreaseReferenceCount(material->metallicMapId);
									material->metallicMapId = 0;
								}
							}
						}
					}

					ImGui::NextColumn();
					//Second column with the widgets
					{
						std::string id_cd("##color_d");
						//id_c.append();
						ImGui::PushID(id_cd.c_str());
						ImGui::ColorEdit4("", material->diffuseColor.ptr(), ImGuiColorEditFlags_NoInputs);
						ImGui::PopID();

						ImGui::NewLine();
						ImGui::NewLine();
						ImGui::NewLine();

						if (material->metallicMapId == 0) {
							std::string id_m("##metalness");
							ImGui::PushID(id_m.c_str());
							ImGui::SliderFloat("", &material->metallic, 0, 1);
							ImGui::PopID();
						} else {
							ImGui::NewLine();
						}
					}
					ImGui::EndColumns();
					ImGui::BeginColumns("##material", 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
					{
						ImGui::Text("Smoothness");
						// Smoothness combo
						const char* smoothnessItems[] = {"Diffuse Alpha", "Specular Alpha"};
						const char* smoothnessItemCurrent = smoothnessItems[material->hasSmoothnessInAlphaChannel];
						if (ImGui::BeginCombo("##smoothness", smoothnessItemCurrent)) {
							for (int n = 0; n < IM_ARRAYSIZE(smoothnessItems); ++n) {
								bool isSelected = (smoothnessItemCurrent == smoothnessItems[n]);
								if (ImGui::Selectable(smoothnessItems[n], isSelected)) {
									material->hasSmoothnessInAlphaChannel = n;
								}
								if (isSelected) {
									ImGui::SetItemDefaultFocus();
								}
							}
							ImGui::EndCombo();
						}
					}
					ImGui::NextColumn();
					{
						ImGui::NewLine();
						std::string id_s("##smooth_");
						ImGui::PushID(id_s.c_str());
						ImGui::SliderFloat(id_s.c_str(), &material->smoothness, 0, 1);
						ImGui::PopID();
					}
					ImGui::EndColumns();
					// TODO: Normal map
				}
			}
		}
		ImGui::TreePop();
	}
}

void ComponentMeshRenderer::Init() {
	ResourceMesh* mesh = App->resources->GetResource<ResourceMesh>(meshId);
	if (!mesh) return;

	palette.resize(mesh->numBones);
	for (unsigned i = 0; i < mesh->numBones; ++i) {
		palette[i] = float4x4::identity;
	}
}

void ComponentMeshRenderer::Update() {
	ResourceMesh* mesh = App->resources->GetResource<ResourceMesh>(meshId);
	if (!mesh) return;

	if (palette.empty()) {
		palette.resize(mesh->numBones);
		for (unsigned i = 0; i < mesh->numBones; ++i) {
			palette[i] = float4x4::identity;
		}
	}

	if (App->time->GetDeltaTime() <= 0) return;

	const GameObject* parent = GetOwner().GetParent();
	const GameObject* rootBone = parent->GetRootBone();
	if (rootBone != nullptr) {
		const GameObject* rootBoneParent = parent->GetRootBone()->GetParent();
		const float4x4& invertedRootBoneTransform = (rootBoneParent != nullptr) ? rootBoneParent->GetComponent<ComponentTransform>()->GetGlobalMatrix().Inverted() : float4x4::identity;

		for (unsigned i = 0; i < mesh->numBones; ++i) {
			const GameObject* bone = goBones.at(mesh->bones[i].boneName);
			palette[i] = invertedRootBoneTransform * bone->GetComponent<ComponentTransform>()->GetGlobalMatrix() * mesh->bones[i].transform;
		}
	}
}

void ComponentMeshRenderer::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_MESH_ID] = meshId;
	jComponent[JSON_TAG_MATERIAL_ID] = materialId;
}

void ComponentMeshRenderer::Load(JsonValue jComponent) {
	meshId = jComponent[JSON_TAG_MESH_ID];
	if (meshId != 0) App->resources->IncreaseReferenceCount(meshId);
	materialId = jComponent[JSON_TAG_MATERIAL_ID];
	if (materialId != 0) App->resources->IncreaseReferenceCount(materialId);
}

void ComponentMeshRenderer::DuplicateComponent(GameObject& owner) {
	ComponentMeshRenderer* component = owner.CreateComponent<ComponentMeshRenderer>();
	component->meshId = meshId;
	component->materialId = materialId;
}

void ComponentMeshRenderer::Draw(const float4x4& modelMatrix) const {
	if (!IsActiveInHierarchy()) return;

	ResourceMesh* mesh = App->resources->GetResource<ResourceMesh>(meshId);
	if (mesh == nullptr) return;

	ResourceMaterial* material = App->resources->GetResource<ResourceMaterial>(materialId);
	if (material == nullptr) return;

	ResourceShader* shader = App->resources->GetResource<ResourceShader>(material->shaderId);
	if (shader == nullptr) return;

	// Light settings
	ComponentLight* directionalLight = nullptr;
	ComponentLight* pointLightsArray[8];
	float pointDistancesArray[8];
	unsigned pointLightsArraySize = 0;
	ComponentLight* spotLightsArray[8];
	float spotDistancesArray[8];
	unsigned spotLightsArraySize = 0;

	float farPointDistance = 0;
	ComponentLight* farPointLight = nullptr;
	float farSpotDistance = 0;
	ComponentLight* farSpotLight = nullptr;

	for (ComponentLight& light : GetOwner().scene->lightComponents) {
		if (light.lightType == LightType::DIRECTIONAL) {
			// It takes the first actived Directional Light inside the Pool
			if (light.IsActiveInHierarchy() && directionalLight == nullptr) {
				directionalLight = &light;
				continue;
			}
		} else if (light.lightType == LightType::POINT) {
			if (light.IsActiveInHierarchy()) {
				float3 meshPosition = GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition();
				float3 lightPosition = light.GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition();
				float distance = Distance(meshPosition, lightPosition);
				if (pointLightsArraySize < 8) {
					pointDistancesArray[pointLightsArraySize] = distance;
					pointLightsArray[pointLightsArraySize] = &light;
					pointLightsArraySize += 1;

					if (distance > farPointDistance) {
						farPointLight = &light;
						farPointDistance = distance;
					}
				} else {
					if (distance < farPointDistance) {
						int count = 0;
						int selected = -1;
						for (float pointDistance : pointDistancesArray) {
							if (pointDistance == farPointDistance) selected = count;
							count += 1;
						}

						pointLightsArray[selected] = &light;
						pointDistancesArray[selected] = distance;

						count = 0;
						selected = -1;
						float maxDistance = 0;
						for (float pointDistance : pointDistancesArray) {
							if (pointDistance > maxDistance) {
								maxDistance = pointDistance;
								selected = count;
							}
							count += 1;
						}

						farPointDistance = maxDistance;
						farPointLight = pointLightsArray[selected];
					}
				}
			}
		} else if (light.lightType == LightType::SPOT) {
			if (light.IsActiveInHierarchy()) {
				float3 meshPosition = GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition();
				float3 lightPosition = light.GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition();
				float distance = Distance(meshPosition, lightPosition);
				if (spotLightsArraySize < 8) {
					spotDistancesArray[spotLightsArraySize] = distance;
					spotLightsArray[spotLightsArraySize] = &light;
					spotLightsArraySize += 1;

					if (distance > farSpotDistance) {
						farSpotLight = &light;
						farSpotDistance = distance;
					}
				} else {
					if (distance < farSpotDistance) {
						int count = 0;
						int selected = -1;
						for (float spotDistance : spotDistancesArray) {
							if (spotDistance == farSpotDistance) selected = count;
							count += 1;
						}

						spotLightsArray[selected] = &light;
						spotDistancesArray[selected] = distance;

						count = 0;
						selected = -1;
						float maxDistance = 0;
						for (float spotDistance : spotDistancesArray) {
							if (spotDistance > maxDistance) {
								maxDistance = spotDistance;
								selected = count;
							}
							count += 1;
						}

						farSpotDistance = maxDistance;
						farSpotLight = spotLightsArray[selected];
					}
				}
			}
		}
	}

	// Common shader settings
	unsigned program = shader->GetShaderProgram();

	float4x4 viewMatrix = App->camera->GetViewMatrix();
	float4x4 projMatrix = App->camera->GetProjectionMatrix();

	unsigned glTextureDiffuse = 0;
	ResourceTexture* diffuse = App->resources->GetResource<ResourceTexture>(material->diffuseMapId);
	glTextureDiffuse = diffuse ? diffuse->glTexture : 0;
	int hasDiffuseMap = diffuse ? 1 : 0;

	if (shader->GetShaderType() == ShaderType::PHONG) {
		// Phong-specific settings
		unsigned glTextureSpecular = 0;
		ResourceTexture* specular = App->resources->GetResource<ResourceTexture>(material->specularMapId);
		glTextureSpecular = specular ? specular->glTexture : 0;
		int hasSpecularMap = specular ? 1 : 0;

		// Phong-specific uniform settings
		glUseProgram(program);

		glUniform3fv(glGetUniformLocation(program, "specularColor"), 1, material->specularColor.ptr());
		glUniform1i(glGetUniformLocation(program, "hasSpecularMap"), hasSpecularMap);
		glUniform1f(glGetUniformLocation(program, "shininess"), material->smoothness);
		glUniform1i(glGetUniformLocation(program, "hasShininessInSpecularAlpha"), material->hasSmoothnessInAlphaChannel);

		glUniform1i(glGetUniformLocation(program, "specularMap"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, glTextureSpecular);

	} else if (shader->GetShaderType() == ShaderType::STANDARD_SPECULAR) {
		// Specular-specific settings
		unsigned glTextureSpecular = 0;
		ResourceTexture* specular = App->resources->GetResource<ResourceTexture>(material->specularMapId);
		glTextureSpecular = specular ? specular->glTexture : 0;
		int hasSpecularMap = specular ? 1 : 0;

		// Specular-specific uniform settings
		glUseProgram(program);

		glUniform3fv(glGetUniformLocation(program, "specularColor"), 1, material->specularColor.ptr());
		glUniform1i(glGetUniformLocation(program, "hasSpecularMap"), hasSpecularMap);
		glUniform1f(glGetUniformLocation(program, "smoothness"), material->smoothness);
		glUniform1i(glGetUniformLocation(program, "hasSpecularAlpha"), material->hasSmoothnessInAlphaChannel);

		glUniform1i(glGetUniformLocation(program, "specularMap"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, glTextureSpecular);

	} else if (shader->GetShaderType() == ShaderType::STANDARD) {
		// Standard-specific settings
		unsigned glTextureMetallic = 0;
		ResourceTexture* metallic = App->resources->GetResource<ResourceTexture>(material->metallicMapId);
		glTextureMetallic = metallic ? metallic->glTexture : 0;
		int hasMetallicMap = metallic ? 1 : 0;

		// Standard-specific uniform settings
		glUseProgram(program);

		glUniform1f(glGetUniformLocation(program, "metalness"), material->metallic);
		glUniform1i(glGetUniformLocation(program, "hasMetallicMap"), hasMetallicMap);
		glUniform1f(glGetUniformLocation(program, "smoothness"), material->smoothness);
		glUniform1i(glGetUniformLocation(program, "hasMetallicAlpha"), material->hasSmoothnessInAlphaChannel);

		glUniform1i(glGetUniformLocation(program, "metallicMap"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, glTextureMetallic);

	} else {
		glUseProgram(program);
	}

	// Common uniform settings
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, modelMatrix.ptr());
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, viewMatrix.ptr());
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, projMatrix.ptr());
	if (palette.size() > 0) {
		glUniformMatrix4fv(glGetUniformLocation(program, "palette"), palette.size(), GL_TRUE, palette[0].ptr());
	}
	glUniform1i(glGetUniformLocation(program, "hasBones"), goBones.size());

	glUniform1i(glGetUniformLocation(program, "diffuseMap"), 0);
	glUniform3fv(glGetUniformLocation(program, "diffuseColor"), 1, material->diffuseColor.ptr());
	glUniform1i(glGetUniformLocation(program, "hasDiffuseMap"), hasDiffuseMap);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glTextureDiffuse);

	// Lights uniforms settings
	glUniform3fv(glGetUniformLocation(program, "light.ambient.color"), 1, App->renderer->ambientColor.ptr());

	if (directionalLight != nullptr) {
		glUniform3fv(glGetUniformLocation(program, "light.directional.direction"), 1, directionalLight->direction.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.directional.color"), 1, directionalLight->color.ptr());
		glUniform1f(glGetUniformLocation(program, "light.directional.intensity"), directionalLight->intensity);
	}
	glUniform1i(glGetUniformLocation(program, "light.directional.isActive"), directionalLight ? 1 : 0);

	if (pointLightsArraySize > 0) {
		glUniform3fv(glGetUniformLocation(program, "light.points[0].pos"), 1, pointLightsArray[0]->pos.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.points[0].color"), 1, pointLightsArray[0]->color.ptr());
		glUniform1f(glGetUniformLocation(program, "light.points[0].intensity"), pointLightsArray[0]->intensity);
		glUniform1f(glGetUniformLocation(program, "light.points[0].kc"), pointLightsArray[0]->kc);
		glUniform1f(glGetUniformLocation(program, "light.points[0].kl"), pointLightsArray[0]->kl);
		glUniform1f(glGetUniformLocation(program, "light.points[0].kq"), pointLightsArray[0]->kq);
	}
	if (pointLightsArraySize > 1) {
		glUniform3fv(glGetUniformLocation(program, "light.points[1].pos"), 1, pointLightsArray[1]->pos.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.points[1].color"), 1, pointLightsArray[1]->color.ptr());
		glUniform1f(glGetUniformLocation(program, "light.points[1].intensity"), pointLightsArray[1]->intensity);
		glUniform1f(glGetUniformLocation(program, "light.points[1].kc"), pointLightsArray[1]->kc);
		glUniform1f(glGetUniformLocation(program, "light.points[1].kl"), pointLightsArray[1]->kl);
		glUniform1f(glGetUniformLocation(program, "light.points[1].kq"), pointLightsArray[1]->kq);
	}
	if (pointLightsArraySize > 2) {
		glUniform3fv(glGetUniformLocation(program, "light.points[2].pos"), 1, pointLightsArray[2]->pos.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.points[2].color"), 1, pointLightsArray[2]->color.ptr());
		glUniform1f(glGetUniformLocation(program, "light.points[2].intensity"), pointLightsArray[2]->intensity);
		glUniform1f(glGetUniformLocation(program, "light.points[2].kc"), pointLightsArray[2]->kc);
		glUniform1f(glGetUniformLocation(program, "light.points[2].kl"), pointLightsArray[2]->kl);
		glUniform1f(glGetUniformLocation(program, "light.points[2].kq"), pointLightsArray[2]->kq);
	}
	if (pointLightsArraySize > 3) {
		glUniform3fv(glGetUniformLocation(program, "light.points[3].pos"), 1, pointLightsArray[3]->pos.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.points[3].color"), 1, pointLightsArray[3]->color.ptr());
		glUniform1f(glGetUniformLocation(program, "light.points[3].intensity"), pointLightsArray[3]->intensity);
		glUniform1f(glGetUniformLocation(program, "light.points[3].kc"), pointLightsArray[3]->kc);
		glUniform1f(glGetUniformLocation(program, "light.points[3].kl"), pointLightsArray[3]->kl);
		glUniform1f(glGetUniformLocation(program, "light.points[3].kq"), pointLightsArray[3]->kq);
	}
	if (pointLightsArraySize > 4) {
		glUniform3fv(glGetUniformLocation(program, "light.points[4].pos"), 1, pointLightsArray[4]->pos.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.points[4].color"), 1, pointLightsArray[4]->color.ptr());
		glUniform1f(glGetUniformLocation(program, "light.points[4].intensity"), pointLightsArray[4]->intensity);
		glUniform1f(glGetUniformLocation(program, "light.points[4].kc"), pointLightsArray[4]->kc);
		glUniform1f(glGetUniformLocation(program, "light.points[4].kl"), pointLightsArray[4]->kl);
		glUniform1f(glGetUniformLocation(program, "light.points[4].kq"), pointLightsArray[4]->kq);
	}
	if (pointLightsArraySize > 5) {
		glUniform3fv(glGetUniformLocation(program, "light.points[5].pos"), 1, pointLightsArray[5]->pos.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.points[5].color"), 1, pointLightsArray[5]->color.ptr());
		glUniform1f(glGetUniformLocation(program, "light.points[5].intensity"), pointLightsArray[5]->intensity);
		glUniform1f(glGetUniformLocation(program, "light.points[5].kc"), pointLightsArray[5]->kc);
		glUniform1f(glGetUniformLocation(program, "light.points[5].kl"), pointLightsArray[5]->kl);
		glUniform1f(glGetUniformLocation(program, "light.points[5].kq"), pointLightsArray[5]->kq);
	}
	if (pointLightsArraySize > 6) {
		glUniform3fv(glGetUniformLocation(program, "light.points[6].pos"), 1, pointLightsArray[6]->pos.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.points[6].color"), 1, pointLightsArray[6]->color.ptr());
		glUniform1f(glGetUniformLocation(program, "light.points[6].intensity"), pointLightsArray[6]->intensity);
		glUniform1f(glGetUniformLocation(program, "light.points[6].kc"), pointLightsArray[6]->kc);
		glUniform1f(glGetUniformLocation(program, "light.points[6].kl"), pointLightsArray[6]->kl);
		glUniform1f(glGetUniformLocation(program, "light.points[6].kq"), pointLightsArray[6]->kq);
	}
	if (pointLightsArraySize > 7) {
		glUniform3fv(glGetUniformLocation(program, "light.points[7].pos"), 1, pointLightsArray[7]->pos.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.points[7].color"), 1, pointLightsArray[7]->color.ptr());
		glUniform1f(glGetUniformLocation(program, "light.points[7].intensity"), pointLightsArray[7]->intensity);
		glUniform1f(glGetUniformLocation(program, "light.points[7].kc"), pointLightsArray[7]->kc);
		glUniform1f(glGetUniformLocation(program, "light.points[7].kl"), pointLightsArray[7]->kl);
		glUniform1f(glGetUniformLocation(program, "light.points[7].kq"), pointLightsArray[7]->kq);
	}
	glUniform1i(glGetUniformLocation(program, "light.numPoints"), pointLightsArraySize);

	if (spotLightsArraySize > 0) {
		glUniform3fv(glGetUniformLocation(program, "light.spots[0].pos"), 1, spotLightsArray[0]->pos.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.spots[0].direction"), 1, spotLightsArray[0]->direction.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.spots[0].color"), 1, spotLightsArray[0]->color.ptr());
		glUniform1f(glGetUniformLocation(program, "light.spots[0].intensity"), spotLightsArray[0]->intensity);
		glUniform1f(glGetUniformLocation(program, "light.spots[0].kc"), spotLightsArray[0]->kc);
		glUniform1f(glGetUniformLocation(program, "light.spots[0].kl"), spotLightsArray[0]->kl);
		glUniform1f(glGetUniformLocation(program, "light.spots[0].kq"), spotLightsArray[0]->kq);
		glUniform1f(glGetUniformLocation(program, "light.spots[0].innerAngle"), spotLightsArray[0]->innerAngle);
		glUniform1f(glGetUniformLocation(program, "light.spots[0].outerAngle"), spotLightsArray[0]->outerAngle);
	}
	if (spotLightsArraySize > 1) {
		glUniform3fv(glGetUniformLocation(program, "light.spots[1].pos"), 1, spotLightsArray[1]->pos.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.spots[1].direction"), 1, spotLightsArray[1]->direction.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.spots[1].color"), 1, spotLightsArray[1]->color.ptr());
		glUniform1f(glGetUniformLocation(program, "light.spots[1].intensity"), spotLightsArray[1]->intensity);
		glUniform1f(glGetUniformLocation(program, "light.spots[1].kc"), spotLightsArray[1]->kc);
		glUniform1f(glGetUniformLocation(program, "light.spots[1].kl"), spotLightsArray[1]->kl);
		glUniform1f(glGetUniformLocation(program, "light.spots[1].kq"), spotLightsArray[1]->kq);
		glUniform1f(glGetUniformLocation(program, "light.spots[1].innerAngle"), spotLightsArray[1]->innerAngle);
		glUniform1f(glGetUniformLocation(program, "light.spots[1].outerAngle"), spotLightsArray[1]->outerAngle);
	}
	if (spotLightsArraySize > 2) {
		glUniform3fv(glGetUniformLocation(program, "light.spots[2].pos"), 1, spotLightsArray[2]->pos.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.spots[2].direction"), 1, spotLightsArray[2]->direction.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.spots[2].color"), 1, spotLightsArray[2]->color.ptr());
		glUniform1f(glGetUniformLocation(program, "light.spots[2].intensity"), spotLightsArray[2]->intensity);
		glUniform1f(glGetUniformLocation(program, "light.spots[2].kc"), spotLightsArray[2]->kc);
		glUniform1f(glGetUniformLocation(program, "light.spots[2].kl"), spotLightsArray[2]->kl);
		glUniform1f(glGetUniformLocation(program, "light.spots[2].kq"), spotLightsArray[2]->kq);
		glUniform1f(glGetUniformLocation(program, "light.spots[2].innerAngle"), spotLightsArray[2]->innerAngle);
		glUniform1f(glGetUniformLocation(program, "light.spots[2].outerAngle"), spotLightsArray[2]->outerAngle);
	}
	if (spotLightsArraySize > 3) {
		glUniform3fv(glGetUniformLocation(program, "light.spots[3].pos"), 1, spotLightsArray[3]->pos.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.spots[3].direction"), 1, spotLightsArray[3]->direction.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.spots[3].color"), 1, spotLightsArray[3]->color.ptr());
		glUniform1f(glGetUniformLocation(program, "light.spots[3].intensity"), spotLightsArray[3]->intensity);
		glUniform1f(glGetUniformLocation(program, "light.spots[3].kc"), spotLightsArray[3]->kc);
		glUniform1f(glGetUniformLocation(program, "light.spots[3].kl"), spotLightsArray[3]->kl);
		glUniform1f(glGetUniformLocation(program, "light.spots[3].kq"), spotLightsArray[3]->kq);
		glUniform1f(glGetUniformLocation(program, "light.spots[3].innerAngle"), spotLightsArray[3]->innerAngle);
		glUniform1f(glGetUniformLocation(program, "light.spots[3].outerAngle"), spotLightsArray[3]->outerAngle);
	}
	if (spotLightsArraySize > 4) {
		glUniform3fv(glGetUniformLocation(program, "light.spots[4].pos"), 1, spotLightsArray[4]->pos.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.spots[4].direction"), 1, spotLightsArray[4]->direction.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.spots[4].color"), 1, spotLightsArray[4]->color.ptr());
		glUniform1f(glGetUniformLocation(program, "light.spots[4].intensity"), spotLightsArray[4]->intensity);
		glUniform1f(glGetUniformLocation(program, "light.spots[4].kc"), spotLightsArray[4]->kc);
		glUniform1f(glGetUniformLocation(program, "light.spots[4].kl"), spotLightsArray[4]->kl);
		glUniform1f(glGetUniformLocation(program, "light.spots[4].kq"), spotLightsArray[4]->kq);
		glUniform1f(glGetUniformLocation(program, "light.spots[4].innerAngle"), spotLightsArray[4]->innerAngle);
		glUniform1f(glGetUniformLocation(program, "light.spots[4].outerAngle"), spotLightsArray[4]->outerAngle);
	}
	if (spotLightsArraySize > 5) {
		glUniform3fv(glGetUniformLocation(program, "light.spots[5].pos"), 1, spotLightsArray[5]->pos.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.spots[5].direction"), 1, spotLightsArray[5]->direction.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.spots[5].color"), 1, spotLightsArray[5]->color.ptr());
		glUniform1f(glGetUniformLocation(program, "light.spots[5].intensity"), spotLightsArray[5]->intensity);
		glUniform1f(glGetUniformLocation(program, "light.spots[5].kc"), spotLightsArray[5]->kc);
		glUniform1f(glGetUniformLocation(program, "light.spots[5].kl"), spotLightsArray[5]->kl);
		glUniform1f(glGetUniformLocation(program, "light.spots[5].kq"), spotLightsArray[5]->kq);
		glUniform1f(glGetUniformLocation(program, "light.spots[5].innerAngle"), spotLightsArray[5]->innerAngle);
		glUniform1f(glGetUniformLocation(program, "light.spots[5].outerAngle"), spotLightsArray[5]->outerAngle);
	}
	if (spotLightsArraySize > 6) {
		glUniform3fv(glGetUniformLocation(program, "light.spots[6].pos"), 1, spotLightsArray[6]->pos.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.spots[6].direction"), 1, spotLightsArray[6]->direction.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.spots[6].color"), 1, spotLightsArray[6]->color.ptr());
		glUniform1f(glGetUniformLocation(program, "light.spots[6].intensity"), spotLightsArray[6]->intensity);
		glUniform1f(glGetUniformLocation(program, "light.spots[6].kc"), spotLightsArray[6]->kc);
		glUniform1f(glGetUniformLocation(program, "light.spots[6].kl"), spotLightsArray[6]->kl);
		glUniform1f(glGetUniformLocation(program, "light.spots[6].kq"), spotLightsArray[6]->kq);
		glUniform1f(glGetUniformLocation(program, "light.spots[6].innerAngle"), spotLightsArray[6]->innerAngle);
		glUniform1f(glGetUniformLocation(program, "light.spots[6].outerAngle"), spotLightsArray[6]->outerAngle);
	}
	if (spotLightsArraySize > 7) {
		glUniform3fv(glGetUniformLocation(program, "light.spots[7].pos"), 1, spotLightsArray[7]->pos.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.spots[7].direction"), 1, spotLightsArray[7]->direction.ptr());
		glUniform3fv(glGetUniformLocation(program, "light.spots[7].color"), 1, spotLightsArray[7]->color.ptr());
		glUniform1f(glGetUniformLocation(program, "light.spots[7].intensity"), spotLightsArray[7]->intensity);
		glUniform1f(glGetUniformLocation(program, "light.spots[7].kc"), spotLightsArray[7]->kc);
		glUniform1f(glGetUniformLocation(program, "light.spots[7].kl"), spotLightsArray[7]->kl);
		glUniform1f(glGetUniformLocation(program, "light.spots[7].kq"), spotLightsArray[7]->kq);
		glUniform1f(glGetUniformLocation(program, "light.spots[7].innerAngle"), spotLightsArray[7]->innerAngle);
		glUniform1f(glGetUniformLocation(program, "light.spots[7].outerAngle"), spotLightsArray[7]->outerAngle);
	}
	glUniform1i(glGetUniformLocation(program, "light.numSpots"), spotLightsArraySize);

	glUniform3fv(glGetUniformLocation(program, "viewPos"), 1, App->camera->GetPosition().ptr());

	glBindVertexArray(mesh->vao);
	glDrawElements(GL_TRIANGLES, mesh->numIndices, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}