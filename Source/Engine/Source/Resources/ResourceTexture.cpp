#include "ResourceTexture.h"

#include "Globals.h"
#include "Application.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleResources.h"
#include "Utils/MSTimer.h"

#include "Utils/Logging.h"
#include "IL/il.h"
#include "IL/ilu.h"
#include "GL/glew.h"

void ResourceTexture::Load() {
	// Timer to measure loading a texture
	MSTimer timer;
	timer.Start();
	std::string filePath = GetResourceFilePath();
	LOG("Loading texture from path: \"%s\".", filePath.c_str());

	// Generate image handler
	unsigned image;
	ilGenImages(1, &image);
	DEFER {
		ilDeleteImages(1, &image);
	};

	// Load image
	ilBindImage(image);
	bool imageLoaded = ilLoad(IL_DDS, filePath.c_str());
	if (!imageLoaded) {
		LOG("Failed to load image.");
		return;
	}

	// Flip image if neccessary
	ILinfo info;
	iluGetImageInfo(&info);
	if (info.Origin == IL_ORIGIN_UPPER_LEFT) {
		iluFlipImage();
	}

	// Generate texture from image
	glGenTextures(1, &glTexture);
	glBindTexture(GL_TEXTURE_2D, glTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

	// Generate mipmaps and set filtering and wrapping
	glGenerateMipmap(GL_TEXTURE_2D);

	unsigned timeMs = timer.Stop();
	LOG("Texture loaded in %ums.", timeMs);
}

void ResourceTexture::Unload() {
	glDeleteTextures(1, &glTexture);
}