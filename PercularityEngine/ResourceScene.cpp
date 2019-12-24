#include "ResourceScene.h"
#include "Application.h"
#include "ModuleScene.h"

bool ResourceScene::LoadInMemory()
{
	App->scene->LoadScene(name, App->scene->sceneAddress, false);
	return true;
}

void ResourceScene::ReleaseFromMemory() 
{
	
}

// Save and Load
void ResourceScene::OnSave(const char* resourceNum, json &config) const
{
	config["Resources"][resourceNum]["UUID"] = UUID;
	config["Resources"][resourceNum]["File"] = file;
	config["Resources"][resourceNum]["Exported file"] = exported_file;
	config["Resources"][resourceNum]["Name"] = name;

	config["Resources"][resourceNum]["Type"] = "Scene";
}

void ResourceScene::OnLoad(const char* resourceNum, const json &config)
{
	UUID = config["Resources"][resourceNum]["UUID"];
	json s1 = config["Resources"][resourceNum]["Name"];
	json s2 = config["Resources"][resourceNum]["File"];
	json s3 = config["Resources"][resourceNum]["Exported file"];

	name = s1.get<std::string>();
	file = s2.get<std::string>();
	exported_file = s3.get<std::string>();
}