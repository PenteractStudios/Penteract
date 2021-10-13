#include "ModuleScene.h"

#include "Globals.h"
#include "Application.h"
#include "GameObject.h"
#include "Utils/Logging.h"
#include "Utils/FileDialog.h"
#include "FileSystem/SceneImporter.h"
#include "FileSystem/TextureImporter.h"
#include "FileSystem/JsonValue.h"
#include "Components/Component.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentLight.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentBoundingBox.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentScript.h"
#include "Components/UI/ComponentCanvas.h"
#include "Components/UI/ComponentCanvasRenderer.h"
#include "Components/UI/ComponentTransform2D.h"
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentProgressBar.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModulePrograms.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleNavigation.h"
#include "Modules/ModuleUserInterface.h"
#include "Modules/ModuleEvents.h"
#include "Modules/ModuleTime.h"
#include "Resources/ResourceTexture.h"
#include "Resources/ResourceSkybox.h"
#include "Resources/ResourceScene.h"
#include "Resources/ResourcePrefab.h"
#include "Panels/PanelHierarchy.h"
#include "Scripting/Script.h"

#include "GL/glew.h"
#include "Math/myassert.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "Math/float4x4.h"
#include "Geometry/Sphere.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/reader.h"
#include "rapidjson/error/en.h"
#include <string>
#include <Windows.h>
#include <array>

#include "Brofiler.h"

#include "Utils/Leaks.h"

static aiLogStream logStream = {nullptr, nullptr};

static void AssimpLogCallback(const char* message, char* user) {
	std::string messageStr = message;
	std::string finalMessageStr = messageStr.substr(0, messageStr.find_last_of('\n'));
	LOG(finalMessageStr.c_str());
}

bool ModuleScene::Init() {
	scene = new Scene(MAX_SCENE_COMPONENTS);

#ifdef _DEBUG
	logStream.callback = AssimpLogCallback;
	aiAttachLogStream(&logStream);
#endif

	return true;
}

bool ModuleScene::Start() {
	App->events->AddObserverToEvent(TesseractEventType::GAMEOBJECT_DESTROYED, this);
	App->events->AddObserverToEvent(TesseractEventType::CHANGE_SCENE, this);
	App->events->AddObserverToEvent(TesseractEventType::COMPILATION_FINISHED, this);
	App->events->AddObserverToEvent(TesseractEventType::PRESSED_PLAY, this);

#if !GAME
	App->files->CreateFolder(ASSETS_PATH);
	App->files->CreateFolder(LIBRARY_PATH);
	App->files->CreateFolder(SKYBOX_PATH);
	App->files->CreateFolder(TEXTURES_PATH);
	App->files->CreateFolder(SHADERS_PATH);
	App->files->CreateFolder(SCENES_PATH);
	App->files->CreateFolder(MATERIALS_PATH);
	App->files->CreateFolder(PREFABS_PATH);
	App->files->CreateFolder(NAVMESH_PATH);
#endif

	CreateEmptyScene();

#if GAME
	App->events->AddEvent(TesseractEventType::PRESSED_PLAY);
	ResourceScene* startScene = App->resources->GetResource<ResourceScene>(startSceneId);
	if (startScene != nullptr) {
		App->scene->LoadScene(startScene->GetResourceFilePath().c_str());
	}

	App->time->SetVSync(true);
	App->time->limitFramerate = false;
	App->renderer->drawDebugDraw = false;
#endif

	return true;
}

UpdateStatus ModuleScene::Update() {
	BROFILER_CATEGORY("ModuleScene - Update", Profiler::Color::Green)

	// Update GameObjects
	scene->root->Update();

	return UpdateStatus::CONTINUE;
}

UpdateStatus ModuleScene::PostUpdate() {
	// Check for scene events
	if (shouldLoadScene) {
		Scene* newScene = SceneImporter::LoadScene(sceneToLoadPath.c_str());
		if (newScene != nullptr) {
			RELEASE(scene);
			scene = newScene;

			ComponentCamera* gameCamera = scene->GetComponent<ComponentCamera>(scene->gameCameraId);
			App->camera->ChangeGameCamera(gameCamera, gameCamera != nullptr);
			App->camera->ChangeActiveCamera(nullptr, false);
			App->camera->ChangeCullingCamera(nullptr, false);

			App->navigation->ChangeNavMesh(scene->navMeshId);

			if (App->time->HasGameStarted()) {
				scene->Start();
			}
		}

		shouldLoadScene = false;
	} else if (shouldSaveScene) {
		SceneImporter::SaveScene(scene, sceneToSavePath.c_str());

		shouldSaveScene = false;
	} else if (shouldBuildPrefab) {
		ResourcePrefab* prefabResource = App->resources->GetResource<ResourcePrefab>(buildingPrefabId);
		if (prefabResource != nullptr) {
			GameObject* parent = scene->GetGameObject(buildingPrefabParentId);
			if (parent != nullptr) {
				UID gameObjectId = prefabResource->BuildPrefab(parent);
				App->editor->selectedGameObject = scene->GetGameObject(gameObjectId);
			}
		}

		App->resources->DecreaseReferenceCount(buildingPrefabId);
		buildingPrefabId = 0;
		buildingPrefabParentId = 0;

		shouldBuildPrefab = false;
	}

	return UpdateStatus::CONTINUE;
}

bool ModuleScene::CleanUp() {
	RELEASE(scene);

#ifdef _DEBUG
	aiDetachAllLogStreams();
#endif

	return true;
}

void ModuleScene::ReceiveEvent(TesseractEvent& e) {
	switch (e.type) {
	case TesseractEventType::GAMEOBJECT_DESTROYED:
		if (e.Get<DestroyGameObjectStruct>().scene != scene) break;

		scene->DestroyGameObject(e.Get<DestroyGameObjectStruct>().gameObject);
		break;
	case TesseractEventType::CHANGE_SCENE: {
		ResourceScene* newScene = App->resources->GetResource<ResourceScene>(e.Get<ChangeSceneStruct>().sceneId);
		if (newScene != nullptr) {
			App->scene->LoadScene(newScene->GetResourceFilePath().c_str());
		}
		break;
	}
	case TesseractEventType::COMPILATION_FINISHED:
		for (ComponentScript& script : scene->scriptComponents) {
			script.CreateScriptInstance();
		}
		break;
	}

}

void ModuleScene::CreateEmptyScene() {
	scene->ClearScene();

	// Create Scene root node
	GameObject* root = scene->CreateGameObject(nullptr, GenerateUID(), "Scene");
	scene->root = root;
	ComponentTransform* sceneTransform = root->CreateComponent<ComponentTransform>();

	// Create Directional Light
	GameObject* dirLight = scene->CreateGameObject(root, GenerateUID(), "Directional Light");
	scene->directionalLight = dirLight;
	ComponentTransform* dirLightTransform = dirLight->CreateComponent<ComponentTransform>();
	dirLightTransform->SetPosition(float3(0, 300, 0));
	dirLightTransform->SetRotation(Quat::FromEulerXYZ(pi / 2, 0.0f, 0.0));
	dirLightTransform->SetScale(float3(1, 1, 1));
	ComponentLight* dirLightLight = dirLight->CreateComponent<ComponentLight>();

	// Create Game Camera
	GameObject* gameCamera = scene->CreateGameObject(root, GenerateUID(), "Game Camera");
	ComponentTransform* gameCameraTransform = gameCamera->CreateComponent<ComponentTransform>();
	gameCameraTransform->SetPosition(float3(2, 3, -5));
	gameCameraTransform->SetRotation(Quat::identity);
	gameCameraTransform->SetScale(float3(1, 1, 1));
	ComponentCamera* gameCameraCamera = gameCamera->CreateComponent<ComponentCamera>();
	ComponentSkyBox* gameCameraSkybox = gameCamera->CreateComponent<ComponentSkyBox>();
	ComponentAudioListener* audioListener = gameCamera->CreateComponent<ComponentAudioListener>();

	scene->Init();
	if (App->time->HasGameStarted()) {
		scene->Start();
	}
}

void ModuleScene::BuildPrefab(UID prefabId, GameObject* parent) {
	if (prefabId == 0) return;
	if (parent == nullptr) return;

	shouldBuildPrefab = true;

	if (buildingPrefabId != prefabId && buildingPrefabParentId != parent->GetID()) {
		App->resources->DecreaseReferenceCount(buildingPrefabId);
		buildingPrefabId = prefabId;
		buildingPrefabParentId = parent->GetID();
		App->resources->IncreaseReferenceCount(buildingPrefabId);
	}
}

void ModuleScene::LoadScene(const char* filePath) {
	shouldLoadScene = true;
	sceneToLoadPath = filePath;
}

void ModuleScene::SaveScene(const char* filePath) {
	shouldSaveScene = true;
	sceneToSavePath = filePath;
}

void ModuleScene::DestroyGameObjectDeferred(GameObject* gameObject) {
	if (gameObject == nullptr) return;

	const std::vector<GameObject*>& children = gameObject->GetChildren();
	for (GameObject* child : children) {
		DestroyGameObjectDeferred(child);
	}
	TesseractEvent e(TesseractEventType::GAMEOBJECT_DESTROYED);
	e.Set<DestroyGameObjectStruct>(gameObject->scene, gameObject);

	App->events->AddEvent(e);
}