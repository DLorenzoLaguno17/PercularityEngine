#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "OpenGL.h"
#include <string>

#include "Brofiler/Lib/Brofiler.h"

#include "mmgr/mmgr.h"

// Called before render is available
bool ModuleResourceManager::Init()
{
	LOG("Preparing resource manager");

	return true;
}

// Called before the first frame
bool ModuleResourceManager::Start()
{
	

	return true;
}

// Called before quitting
bool ModuleResourceManager::CleanUp()
{
	LOG("Freeing resource manager");

	return true;
}