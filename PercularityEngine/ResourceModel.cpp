#include "ResourceModel.h"
#include "Application.h"
#include "ModuleScene.h"

bool ResourceModel::LoadInMemory()
{	
	App->scene->LoadScene(App->scene->GetRoot(), name, App->scene->sceneAddress, true);
	return true;
}

void ResourceModel::ReleaseFromMemory() {}

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