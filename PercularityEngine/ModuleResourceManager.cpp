#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleResourceLoader.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"
#include "GameObject.h"
#include "OpenGL.h"
#include <string>

#include "Brofiler/Lib/Brofiler.h"
#include "ImGui/imgui.h"
#include "mmgr/mmgr.h"

// Called before the first frame
bool ModuleResourceManager::Start()
{
	return true;
}

Resource* ModuleResourceManager::CreateNewResource(RESOURCE_TYPE type, uint specific_uid) {
	
	Resource* ret = nullptr;
	uint uuid;
	
	if (specific_uid != 0 && GetResourceFromMap(specific_uid) == nullptr) uuid = specific_uid;
	else uuid = (uint)App->GetRandomGenerator().Int();

	switch (type) {
		case RESOURCE_TYPE::TEXTURE: 
			ret = (Resource*) new ResourceTexture(uuid); 
			break;
		case RESOURCE_TYPE::MESH: 
			ret = (Resource*) new ResourceMesh(uuid); 
			break;
		//case RESOURCE_TYPE::SCENE: ret = (Resource*) new ResourceScene(uid); break;
	}

	if (ret != nullptr) {
		resources[uuid] = ret;
		resourcesCount++;
	}

	return ret;
}

uint ModuleResourceManager::ReceiveExternalFile(const char* new_file)
{
	uint ret = 0;

	// We create the correct path
	std::string final_path;
	std::string extension;

	// If we don't have iot already, we add the file to our Assets folder
	App->file_system->SplitFilePath(new_file, nullptr, &final_path, &extension);

	if (CheckTextureExtension(extension.c_str()))
		final_path = ASSETS_TEXTURE_FOLDER + final_path;
	if (CheckMeshExtension(extension.c_str()))
		final_path = ASSETS_MODEL_FOLDER + final_path;

	if (App->file_system->CopyFromOutsideFS(new_file, final_path.c_str())) {

		RESOURCE_TYPE type = GetTypeFromExtension(extension.c_str());
		ret = ImportFile(final_path.c_str(), type);
	}

	return ret;
}

uint ModuleResourceManager::ImportFile(const char* new_file, RESOURCE_TYPE type, bool force)
{
	uint ret = 0;

	// Check is the file has already been exported
	if (force)
	{
		ret = FindFileInAssets(new_file);
		if (ret != 0) return ret;
	}

	bool success = false;
	std::string written_file;

	switch (type) {
	case RESOURCE_TYPE::TEXTURE:
		success = App->res_loader->LoadTexture(new_file, written_file);
		break;
	case RESOURCE_TYPE::MESH:
		success = App->res_loader->LoadModel(new_file, written_file);
		break;
	/*case RESOURCE_TYPE::SCENE: 
		success = App->scene->Import(new_file_in_assets, written_file); 
		break;*/
	}

	// If the import was successful, create a new resource
	if (success) {
		Resource* res = CreateNewResource(type);
		res->file = new_file;
		App->file_system->NormalizePath(res->file);
		res->exported_file = written_file;
		res->name = App->res_loader->getNameFromPath(res->file);
		ret = res->GetUUID();
	}
}

// Load & Save
void ModuleResourceManager::LoadResources(const nlohmann::json &config) {
	CleanUp();
}

void ModuleResourceManager::SaveResources(nlohmann::json &scene_file) {
	for (std::map<uint, Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
	{
		saved_res++;
		char name[50];
		sprintf_s(name, 50, "Resource %d", saved_res);
		if (it->second->IsLoadedToMemory())
			it->second->OnSave(name, scene_file);
	}

	saved_res = 0;
}

// Checks if a file already exists in Assets folder, and returns the UUID of the resource if it does
uint ModuleResourceManager::FindFileInAssets(const char* existing_file) const {
	std::string file = existing_file;
	App->file_system->NormalizePath(file);

	for (std::map<uint, Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (it->second->file.compare(file) == 0)
			return it->first;
	}
	return 0;
}

const Resource* ModuleResourceManager::GetResourceFromMap(uint uuid) const
{
	if (resources.find(uuid) != resources.end())
		return resources.at(uuid);

	return nullptr;
}

// Returns a resource with certaing uid from the map
Resource* ModuleResourceManager::GetResourceFromMap(uint uuid)
{
	std::map<uint, Resource*>::iterator it = resources.find(uuid);
	if (it != resources.end())
		return it->second;

	return nullptr;
}

// Called before quitting
bool ModuleResourceManager::CleanUp()
{
	LOG("Freeing resource manager");
	/*for (std::map<uint, Resource*>::iterator item = resources.begin(); item != resources.end(); ++item)
		RELEASE(item->second);*/

	resources.clear();
	resourcesCount = 0;

	return true;
}

void ModuleResourceManager::DrawProjectExplorer() {
	for (std::map<uint, Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (it->second) {
			if (ImGui::Button(it->second->name.c_str())) 
			{
				switch (it->second->type) {
				case RESOURCE_TYPE::MESH:
					GameObject* test = new GameObject(it->second->name.c_str(), App->scene->GetRoot());
					ComponentMesh* mesh = test->GetComponent<ComponentMesh>();
					mesh->resource_mesh = (ResourceMesh*)it->second;
					break;
				}

				it->second->UpdateReferenceCount();
			}
		}
	}
}

// Methods to check the extension of a file
RESOURCE_TYPE ModuleResourceManager::GetTypeFromExtension(const char* extension) 
{	
	if (CheckMeshExtension(extension)) return RESOURCE_TYPE::MESH;
	else if (CheckTextureExtension(extension)) return RESOURCE_TYPE::TEXTURE;
	//else if () return RESOURCE_TYPE::SCENE;
	else return RESOURCE_TYPE::UNKNOWN;
}

bool ModuleResourceManager::CheckTextureExtension(const char* extension) {
	return (strcmp(extension, "dds") == 0 || strcmp(extension, "png") == 0 || strcmp(extension, "jpg") == 0);
}

bool ModuleResourceManager::CheckMeshExtension(const char* extension) {
	return (strcmp(extension, "fbx") == 0 || strcmp(extension, "FBX") == 0);
}