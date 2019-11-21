#include "ResourceModel.h"
#include "ResourceMesh.h"
#include "Application.h"
#include "ModuleResourceLoader.h"
#include "ModuleResourceManager.h"

bool ResourceModel::LoadInMemory()
{	
	bool ret = App->res_loader->LoadModelFromLibrary(this);

	for (int i = 0; i < children_meshes; ++i) {
		ResourceMesh* rm = (ResourceMesh*)App->res_manager->CreateNewResource(RESOURCE_TYPE::MESH);
		rm->UpdateReferenceCount();
		meshes.push_back(rm);
	}

	return ret;
}

void ResourceModel::ReleaseFromMemory() 
{
	for (int i = 0; i < meshes.size(); ++i)
		meshes[i]->ReleaseFromMemory();
}

// Save and Load
void ResourceModel::OnSave(const char* resourceNum, json &config) const
{
	config["Resources"]["Model resources"][resourceNum]["UUID"] = UUID;
	config["Resources"]["Model resources"][resourceNum]["File"] = file;
	config["Resources"]["Model resources"][resourceNum]["Exported file"] = exported_file;
	config["Resources"]["Model resources"][resourceNum]["Name"] = name;
}

void ResourceModel::OnLoad(const char* resourceNum, const json &config)
{
	UUID = config["Resources"]["Model resources"][resourceNum]["UUID"];
	json s1 = config["Resources"]["Model resources"][resourceNum]["File"];
	json s2 = config["Resources"]["Model resources"][resourceNum]["Exported file"];
	json s3 = config["Resources"]["Model resources"][resourceNum]["Name"];

	file = s1.get<std::string>();
	exported_file = s2.get<std::string>();
	name = s3.get<std::string>();
}