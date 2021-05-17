#include "PrefabImporter.h"

#include "Application.h"
#include "GameObject.h"
#include "Utils/Logging.h"
#include "Utils/MSTimer.h"
#include "Utils/FileDialog.h"
#include "Resources/ResourcePrefab.h"
#include "Modules/ModuleResources.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/error/en.h"

#include "Utils/Leaks.h"

#define JSON_TAG_RESOURCES "Resources"
#define JSON_TAG_TYPE "Type"
#define JSON_TAG_ID "Id"
#define JSON_TAG_ROOT "Root"

bool PrefabImporter::ImportPrefab(const char* filePath, JsonValue jMeta) {
	// Timer to measure importing a prefab
	MSTimer timer;
	timer.Start();
	LOG("Importing prefab from path: \"%s\".", filePath);

	// Read from file
	Buffer<char> buffer = App->files->Load(filePath);
	if (buffer.Size() == 0) {
		LOG("Error loading prefab %s", filePath);
		return false;
	}

	// Parse document from file
	rapidjson::Document document;
	document.ParseInsitu<rapidjson::kParseNanAndInfFlag>(buffer.Data());
	if (document.HasParseError()) {
		LOG("Error parsing JSON: %s (offset: %u)", rapidjson::GetParseError_En(document.GetParseError()), document.GetErrorOffset());
		return false;
	}

	// Write document to buffer
	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer, rapidjson::UTF8<>, rapidjson::UTF8<>, rapidjson::CrtAllocator, rapidjson::kWriteNanAndInfFlag> writer(stringBuffer);
	document.Accept(writer);

	// Create prefab resource
	JsonValue jResources = jMeta[JSON_TAG_RESOURCES];
	JsonValue jResource = jResources[0];
	UID metaId = jResource[JSON_TAG_ID];
	UID id = metaId ? metaId : GenerateUID();
	App->resources->CreateResource<ResourcePrefab>(filePath, id);

	// Add resource to meta file
	jResource[JSON_TAG_TYPE] = GetResourceTypeName(ResourcePrefab::staticType);
	jResource[JSON_TAG_ID] = id;

	// Save to file
	App->files->Save(App->resources->GenerateResourcePath(id).c_str(), stringBuffer.GetString(), stringBuffer.GetSize());

	unsigned timeMs = timer.Stop();
	LOG("Prefab imported in %ums", timeMs);
	return true;
}

bool PrefabImporter::SavePrefab(const char* filePath, GameObject* root) {
	// Timer to measure saving a prefab
	MSTimer timer;
	timer.Start();
	LOG("Saving prefab to path: \"%s\".", filePath);

	// Create document
	rapidjson::Document document;
	document.SetObject();
	JsonValue jScene(document, document);

	// Save GameObjects
	JsonValue jRoot = jScene[JSON_TAG_ROOT];
	root->SavePrefab(jRoot);

	// Write document to buffer
	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer, rapidjson::UTF8<>, rapidjson::UTF8<>, rapidjson::CrtAllocator, rapidjson::kWriteNanAndInfFlag> writer(stringBuffer);
	document.Accept(writer);

	// Save to file
	bool saved = App->files->Save(filePath, stringBuffer.GetString(), stringBuffer.GetSize());
	if (!saved) {
		LOG("Failed to save prefab resource.");
		return false;
	}

	unsigned timeMs = timer.Stop();
	LOG("Prefab saved in %ums", timeMs);
	return true;
}
