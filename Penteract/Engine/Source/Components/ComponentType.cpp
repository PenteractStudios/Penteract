#include "ComponentType.h"

#include "Utils/Logging.h"

#include "Math/myassert.h"

#include "Utils/Leaks.h"

const char* GetComponentTypeName(ComponentType type) {
	switch (type) {
	case ComponentType::TRANSFORM:
		return "Transform";
	case ComponentType::MESH_RENDERER:
		return "MeshRenderer";
	case ComponentType::BOUNDING_BOX:
		return "BoundingBox";
	case ComponentType::CAMERA:
		return "Camera";
	case ComponentType::LIGHT:
		return "Light";
	case ComponentType::CANVAS:
		return "Canvas";
	case ComponentType::CANVASRENDERER:
		return "CanvasRenderer";
	case ComponentType::IMAGE:
		return "Image";
	case ComponentType::TRANSFORM2D:
		return "Transform2D";
	case ComponentType::BUTTON:
		return "Button";
	case ComponentType::EVENT_SYSTEM:
		return "EventSystem";
	case ComponentType::BOUNDING_BOX_2D:
		return "BoundingBox2D";
	case ComponentType::TOGGLE:
		return "Toggle";
	case ComponentType::SELECTABLE:
		return "Selectable";
	case ComponentType::SLIDER:
		return "Slider";
	case ComponentType::SKYBOX:
		return "Skybox";
	case ComponentType::ANIMATION:
		return "Animation";
	case ComponentType::TEXT:
		return "Text";
	case ComponentType::SCRIPT:
		return "Script";
	case ComponentType::TRAIL:
		return "Trail";
	case ComponentType::PARTICLE:
		return "Particle";
	case ComponentType::BILLBOARD:
		return "Billboard";
	case ComponentType::AUDIO_LISTENER:
		return "AudioListener";
	case ComponentType::AUDIO_SOURCE:
		return "AudioSource";
	case ComponentType::PROGRESS_BAR:
		return "ProgressBar";
	case ComponentType::SPHERE_COLLIDER:
		return "SphereCollider";
	case ComponentType::BOX_COLLIDER:
		return "BoxCollider";
	case ComponentType::CAPSULE_COLLIDER:
		return "CapsuleCollider";
	case ComponentType::AGENT:
		return "Agent";
	case ComponentType::OBSTACLE:
		return "Obstacle";
	case ComponentType::FOG:
		return "Fog";
	default:
		LOG("Component of type %i hasn't been registered in GetComponentTypeName.", (unsigned) type);
		assert(false); // ERROR: Component type not registered
		return nullptr;
	}
}

ComponentType GetComponentTypeFromName(const char* name) {
	if (strcmp(name, "Transform") == 0) {
		return ComponentType::TRANSFORM;
	} else if (strcmp(name, "MeshRenderer") == 0) {
		return ComponentType::MESH_RENDERER;
	} else if (strcmp(name, "BoundingBox") == 0) {
		return ComponentType::BOUNDING_BOX;
	} else if (strcmp(name, "Camera") == 0) {
		return ComponentType::CAMERA;
	} else if (strcmp(name, "Light") == 0) {
		return ComponentType::LIGHT;
	} else if (strcmp(name, "Canvas") == 0) {
		return ComponentType::CANVAS;
	} else if (strcmp(name, "CanvasRenderer") == 0) {
		return ComponentType::CANVASRENDERER;
	} else if (strcmp(name, "Image") == 0) {
		return ComponentType::IMAGE;
	} else if (strcmp(name, "Transform2D") == 0) {
		return ComponentType::TRANSFORM2D;
	} else if (strcmp(name, "Button") == 0) {
		return ComponentType::BUTTON;
	} else if (strcmp(name, "EventSystem") == 0) {
		return ComponentType::EVENT_SYSTEM;
	} else if (strcmp(name, "BoundingBox2D") == 0) {
		return ComponentType::BOUNDING_BOX_2D;
	} else if (strcmp(name, "Toggle") == 0) {
		return ComponentType::TOGGLE;
	} else if (strcmp(name, "Selectable") == 0) {
		return ComponentType::SELECTABLE;
	} else if (strcmp(name, "Slider") == 0) {
		return ComponentType::SLIDER;
	} else if (strcmp(name, "Skybox") == 0) {
		return ComponentType::SKYBOX;
	} else if (strcmp(name, "Animation") == 0) {
		return ComponentType::ANIMATION;
	} else if (strcmp(name, "Text") == 0) {
		return ComponentType::TEXT;
	} else if (strcmp(name, "Script") == 0) {
		return ComponentType::SCRIPT;
	} else if (strcmp(name, "Particle") == 0) {
		return ComponentType::PARTICLE;
	} else if (strcmp(name, "Trail") == 0) {
		return ComponentType::TRAIL;
	} else if (strcmp(name, "Billboard") == 0) {
		return ComponentType::BILLBOARD;
	} else if (strcmp(name, "AudioListener") == 0) {
		return ComponentType::AUDIO_LISTENER;
	} else if (strcmp(name, "AudioSource") == 0) {
		return ComponentType::AUDIO_SOURCE;
	} else if (strcmp(name, "ProgressBar") == 0) {
		return ComponentType::PROGRESS_BAR;
	} else if (strcmp(name, "SphereCollider") == 0) {
		return ComponentType::SPHERE_COLLIDER;
	} else if (strcmp(name, "BoxCollider") == 0) {
		return ComponentType::BOX_COLLIDER;
	} else if (strcmp(name, "CapsuleCollider") == 0) {
		return ComponentType::CAPSULE_COLLIDER;
	} else if (strcmp(name, "Agent") == 0) {
		return ComponentType::AGENT;
	} else if (strcmp(name, "Obstacle") == 0) {
		return ComponentType::OBSTACLE;
	} else if (strcmp(name, "Fog") == 0) {
		return ComponentType::FOG;
	} else {
		LOG("No component of name %s exists.", (unsigned) name);
		assert(false); // ERROR: Invalid name
		return ComponentType::UNKNOWN;
	}
}