#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleResourceLoader.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ResourceMesh.h"
#include "ResourceModel.h"
#include "ResourceTexture.h"
#include "ResourceScene.h"
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

Resource* ModuleResourceManager::CreateNewResource(RESOURCE_TYPE type, uint specific_uuid) {
	
	Resource* ret = nullptr;
	uint uuid;
	
	if (specific_uuid != 0 && GetResourceFromMap(specific_uuid) == nullptr) uuid = specific_uuid;
	else uuid = (uint)App->GetRandomGenerator().Int();

	switch (type) {
		case RESOURCE_TYPE::TEXTURE: 
			ret = (Resource*) new ResourceTexture(uuid); 
			break;
		case RESOURCE_TYPE::MODEL:
			ret = (Resource*) new ResourceModel(uuid);
			break;
		case RESOURCE_TYPE::MESH: 
			ret = (Resource*) new ResourceMesh(uuid); 
			break;
		case RESOURCE_TYPE::SCENE: 
			ret = (Resource*) new ResourceScene(uuid); 
			break;
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
	else if (CheckModelExtension(extension.c_str()))
		final_path = ASSETS_MODEL_FOLDER + final_path;
	else if (strcmp(extension.c_str(), "json") == 0)
		final_path = ASSETS_SCENE_FOLDER + final_path;

	if (App->file_system->CopyFromOutsideFS(new_file, final_path.c_str())) {

		RESOURCE_TYPE type = GetTypeFromExtension(extension.c_str());
		ret = ImportFile(final_path.c_str(), type, true);
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
	case RESOURCE_TYPE::MODEL:
		success = App->res_loader->LoadModel(new_file, written_file);
		break;
	case RESOURCE_TYPE::SCENE: 
		success = true; 
		break;
	}

	// If the import was successful, we create a new resource
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
void ModuleResourceManager::LoadResources(const json &scene_file) 
{
	CleanUp();

	uint cnt = scene_file["Resources"]["Count"];
	for (int i = 1; i <= cnt; ++i) {
		char name[50];
		sprintf_s(name, 50, "Resource %d", i);
		uint UUID = scene_file["Resources"][name]["UUID"];

		if (GetResourceFromMap(UUID) == nullptr) {
			json js = scene_file["Resources"][name]["Type"];
			std::string type = js.get<std::string>();
			
			if (strcmp(type.c_str(), "Mesh") == 0) {
				ResourceMesh* res = (ResourceMesh*)CreateNewResource(RESOURCE_TYPE::MESH, UUID);
				res->OnLoad(name, scene_file);
				res->UpdateReferenceCount();
			}
			if (strcmp(type.c_str(), "Texture") == 0) {
				ResourceTexture* res = (ResourceTexture*)CreateNewResource(RESOURCE_TYPE::TEXTURE, UUID);
				res->OnLoad(name, scene_file);
				res->UpdateReferenceCount();
			}
		}
	}
}

void ModuleResourceManager::SaveResources(json &scene_file) 
{
	for (std::map<uint, Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (it->second->file.compare("None")) {
			saved_res++;
			char name[50];
			sprintf_s(name, 50, "Resource %d", saved_res);
			it->second->OnSave(name, scene_file);
		}

		scene_file["Resources"]["Count"] = saved_res;
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

uint ModuleResourceManager::FindFileInLibrary(const char* exported_file) const {
	std::string file = exported_file;
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
	uint i = uuid;
	if (it != resources.end())
		return it->second;

	return nullptr;
}

// Called before quitting
bool ModuleResourceManager::CleanUp()
{
	LOG("Freeing resource manager");
	for (std::map<uint, Resource*>::iterator item = resources.begin(); item != resources.end(); ++item) {
		
		if (strcmp(item._Ptr->_Myval.second->file.c_str(), "None") != 0) {
			item._Ptr->_Myval.second->ReleaseFromMemory();
			RELEASE(item._Ptr->_Myval.second);
		}
	}

	resources.clear();
	resourcesCount = 0;

	return true;
}

void ModuleResourceManager::DrawProjectExplorer() {
	
	std::vector<std::string> tex_files;
	std::vector<std::string> tex_directories;
	std::vector<std::string> mod_files;
	std::vector<std::string> mod_directories;
	std::vector<std::string> sce_files;
	std::vector<std::string> sce_directories;

	App->file_system->DiscoverFiles(ASSETS_TEXTURE_FOLDER, tex_files, tex_directories);
	App->file_system->DiscoverFiles(ASSETS_MODEL_FOLDER, mod_files, mod_directories);
	App->file_system->DiscoverFiles(ASSETS_SCENE_FOLDER, sce_files, sce_directories);

	for (int i = 0; i < tex_files.size(); ++i) {
		if (ImGui::ImageButton((void*)App->res_loader->tex_icon_tex->texture, ImVec2(50, 50))) {

			/*std::string path = ASSETS_TEXTURE_FOLDER + tex_files[i];
			ComponentMaterial* mat = (ComponentMaterial*)App->scene->selected->GetComponent(COMPONENT_TYPE::MATERIAL);
			if (mat == nullptr) mat = (ComponentMaterial*)App->scene->selected->CreateComponent(COMPONENT_TYPE::MATERIAL);
			mat->resource_tex = (ResourceTexture*)GetResourceFromMap(FindFileInAssets(path.c_str()));*/

		}
		ImGui::Text(tex_files[i].c_str());
		break;
	}

	for (int i = 0; i < mod_files.size(); ++i) {
		if (ImGui::ImageButton((void*)App->res_loader->model_icon_tex->texture, ImVec2(50, 50))) {

			/*ComponentMaterial* mat = (ComponentMaterial*)App->scene->selected->GetComponent(COMPONENT_TYPE::MATERIAL);
			if (mat) mat->resource_tex->usedAsReference--;
			if (mat == nullptr) mat = (ComponentMaterial*)App->scene->selected->CreateComponent(COMPONENT_TYPE::MATERIAL);
			mat->resource_tex = (ResourceTexture*)it->second;*/
		}
		ImGui::Text(mod_files[i].c_str());
		break;
	}

	for (int i = 0; i < sce_files.size(); ++i) {
		if (ImGui::ImageButton((void*)App->res_loader->scene_icon_tex->texture, ImVec2(50, 50))) {

			//App->scene->LoadScene(App->scene->GetRoot(), "Scene", App->scene->sceneAddress);
		}
		ImGui::Text(sce_files[i].c_str());
		break;
	}
}

// Methods to check the extension of a file
RESOURCE_TYPE ModuleResourceManager::GetTypeFromExtension(const char* extension) 
{	
	if (CheckModelExtension(extension)) return RESOURCE_TYPE::MODEL;
	else if (CheckTextureExtension(extension)) return RESOURCE_TYPE::TEXTURE;
	else if (strcmp(extension, "json") == 0) return RESOURCE_TYPE::SCENE;
	else return RESOURCE_TYPE::UNKNOWN;
}

bool ModuleResourceManager::CheckTextureExtension(const char* extension) {
	return (strcmp(extension, "dds") == 0 || strcmp(extension, "png") == 0 || strcmp(extension, "jpg") == 0);
}

bool ModuleResourceManager::CheckModelExtension(const char* extension) {
	return (strcmp(extension, "fbx") == 0 || strcmp(extension, "FBX") == 0 || strcmp(extension, "obj") == 0);
}