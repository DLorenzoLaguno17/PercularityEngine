#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "OpenGL.h"
#include <string>

#include "Brofiler/Lib/Brofiler.h"
#include "mmgr/mmgr.h"

// Called before the first frame
bool ModuleResourceManager::Start()
{
	return true;
}

Resource* ModuleResourceManager::CreateNewResource(RESOURCE_TYPE type, UID force_uid) {
	
	Resource* ret = nullptr;
	UID uuid;
	
	/*if (force_uid != 0 && Get(force_uid) == nullptr) uuid = force_uid;
	else uuid = GenerateNewUID();

	switch (type) {
		case RESOURCE_TYPE::TEXTURE: ret = (Resource*) new ResourceTexture(uuid); break;
		case RESOURCE_TYPE::MESH: ret = (Resource*) new ResourceMesh(uuid); break;
		case RESOURCE_TYPE::SCENE: ret = (Resource*) new ResourceScene(uuid); break;
	}*/

	if (ret != nullptr)	resources[uuid] = ret;
	return ret;
}

UID ModuleResourceManager::ImportFile(const char* new_file_in_assets, RESOURCE_TYPE type, bool force)
{
	UID ret = 0; 
	bool success = false; 
	std::string written_file;
	
	/*switch (type) {
		case RESOURCE_TYPE::TEXTURE: 
			success = App->tex->Import(new_file_in_assets, "", written_file); 
			break;
		case RESOURCE_TYPE::MESH: success = 
			App->audio->Import(new_file_in_assets, written_file); 
			break;
		case RESOURCE_TYPE::SCENE: success = 
			App->scene->Import(new_file_in_assets, written_file); 
			break;
	}

	// If import was successful, create a new resource
	if (success) { 
		Resource* res = CreateNewResource(type);
		res->file = new_file_in_assets;
		res->exported_file = written_file;
		ret = res->GetUID();
	}*/

	return ret;
}

// Returns a resource with certaing uid from the map
Resource* ModuleResourceManager::Get(UID uid)
{
	std::map<UID, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
		return it->second;

	return nullptr;
}

// Called before quitting
bool ModuleResourceManager::CleanUp()
{
	LOG("Freeing resource manager");
	for (std::map<UID, Resource*>::iterator item = resources.begin(); item != resources.end(); ++item)
		RELEASE(item->second);

	resources.clear();

	return true;
}