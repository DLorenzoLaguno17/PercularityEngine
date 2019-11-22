#include "ResourceModel.h"
#include "ResourceMesh.h"
#include "Application.h"
#include "ModuleResourceLoader.h"
#include "ModuleResourceManager.h"

bool ResourceModel::LoadInMemory()
{	
	bool ret = false;

	for (int i = 0; i < meshes.size(); ++i) {
		ret = meshes[i]->UpdateReferenceCount();
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
	config["Resources"][resourceNum]["UUID"] = UUID;
	config["Resources"][resourceNum]["File"] = file;
	config["Resources"][resourceNum]["Exported file"] = exported_file;
	config["Resources"][resourceNum]["Name"] = name;
	config["Resources"][resourceNum]["Type"] = "Model";
}

void ResourceModel::OnLoad(const char* resourceNum, const json &config)
{
	UUID = config["Resources"][resourceNum]["UUID"];
	json s1 = config["Resources"][resourceNum]["File"];
	json s2 = config["Resources"][resourceNum]["Exported file"];
	json s3 = config["Resources"][resourceNum]["Name"];

	file = s1.get<std::string>();
	exported_file = s2.get<std::string>();
	name = s3.get<std::string>();
}