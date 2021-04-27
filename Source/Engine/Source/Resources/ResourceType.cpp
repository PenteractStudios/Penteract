#include "ResourceType.h"

#include "Utils/Logging.h"

#include "Math/myassert.h"

#include "Utils/Leaks.h"

const char* GetResourceTypeName(ResourceType type) {
	switch (type) {
	case ResourceType::MATERIAL:
		return "Material";
	case ResourceType::MESH:
		return "Mesh";
	case ResourceType::PREFAB:
		return "Prefab";
	case ResourceType::SCENE:
		return "Scene";
	case ResourceType::SHADER:
		return "Shader";
	case ResourceType::TEXTURE:
		return "Texture";
	case ResourceType::FONT:
		return "Font";
	case ResourceType::SKYBOX:
		return "Skybox";
	case ResourceType::SCRIPT:
		return "Script";
	case ResourceType::ANIMATION:
		return "Animation";
	case ResourceType::AUDIO:
		return "Audio";
	default:
		LOG("Resource of type %i hasn't been registered in GetResourceTypeName.", (unsigned) type);
		assert(false); // ERROR: Resource type not registered
		return nullptr;
	}
}

ResourceType GetResourceTypeFromName(const char* name) {
	if (strcmp(name, "Material") == 0) {
		return ResourceType::MATERIAL;
	} else if (strcmp(name, "Mesh") == 0) {
		return ResourceType::MESH;
	} else if (strcmp(name, "Prefab") == 0) {
		return ResourceType::PREFAB;
	} else if (strcmp(name, "Scene") == 0) {
		return ResourceType::SCENE;
	} else if (strcmp(name, "Shader") == 0) {
		return ResourceType::SHADER;
	} else if (strcmp(name, "Texture") == 0) {
		return ResourceType::TEXTURE;
	} else if (strcmp(name, "Font") == 0) {
		return ResourceType::FONT;
	} else if (strcmp(name, "Skybox") == 0) {
		return ResourceType::SKYBOX;
	} else if (strcmp(name, "Script") == 0) {
		return ResourceType::SCRIPT;
	} else if (strcmp(name, "Animation") == 0) {
		return ResourceType::ANIMATION;
	} else if (strcmp(name, "Audio") == 0) {
		return ResourceType::AUDIO;
	} else {
		LOG("No resource of name %s exists.", (unsigned) name);
		assert(false); // ERROR: Invalid name
		return ResourceType::UNKNOWN;
	}
}