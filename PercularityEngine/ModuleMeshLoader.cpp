#include "Application.h"
#include "ModuleMeshLoader.h"

/*#include "OpenGL.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"*/
#include "Brofiler/Lib/Brofiler.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/lib86/assimp.lib")


ModuleMeshLoader::ModuleMeshLoader(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

// Destructor
ModuleMeshLoader::~ModuleMeshLoader()
{}

// Called before render is available
bool ModuleMeshLoader::Awake()
{
	LOG("Preparing mesh loader");

	return true;
}

// Called before the first frame
bool ModuleMeshLoader::Start()
{
	
	return true;
}

// Called before Update
update_status ModuleMeshLoader::PreUpdate(float dt)
{
	BROFILER_CATEGORY("MeshLoaderPreUpdate", Profiler::Color::Orange)

	return UPDATE_CONTINUE;
}

// Called every frame
update_status ModuleMeshLoader::Update(float dt)
{
	BROFILER_CATEGORY("MeshLoaderUpdate", Profiler::Color::LightSeaGreen)

	return UPDATE_CONTINUE;
}

// Called after Update
update_status ModuleMeshLoader::PostUpdate(float dt)
{
	BROFILER_CATEGORY("MeshLoaderPostUpdate", Profiler::Color::Yellow)
	
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleMeshLoader::CleanUp()
{
	LOG("Freeing mesh loader");

	return true;
}

void ModuleMeshLoader::Load(const json &config)
{
}

void ModuleMeshLoader::Save(json &config)
{
}