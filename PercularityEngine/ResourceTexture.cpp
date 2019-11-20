#include "ResourceTexture.h"
#include "Application.h"
#include "ModuleResourceLoader.h"

bool ResourceTexture::LoadInMemory()
{
	if (strcmp(file.c_str(), "None") != 0)		
		return App->res_loader->LoadTextureFromLibrary(exported_file.c_str(), this);
	else
		return false;
}

void ResourceTexture::ReleaseFromMemory()
{}

// Save and Load
void ResourceTexture::OnSave(const char* resourceNum, nlohmann::json &config) const
{
	config["Resources"]["Mesh resources"][resourceNum]["UUID"] = UUID;
	config["Resources"]["Mesh resources"][resourceNum]["File"] = file;
	config["Resources"]["Mesh resources"][resourceNum]["Exported file"] = exported_file;
	config["Resources"]["Mesh resources"][resourceNum]["Name"] = name;
	config["Resources"]["Mesh resources"][resourceNum]["Width"] = width;
	config["Resources"]["Mesh resources"][resourceNum]["Height"] = height;
	config["Resources"]["Mesh resources"][resourceNum]["Texture"] = texture;
}

void ResourceTexture::OnLoad(const char* resourceNum, const nlohmann::json &config)
{
	UUID = config["Resources"]["Mesh resources"][resourceNum]["UUID"];
	width = config["Resources"]["Mesh resources"][resourceNum]["Width"];
	height = config["Resources"]["Mesh resources"][resourceNum]["Height"];
	texture = config["Resources"]["Mesh resources"][resourceNum]["Texture"];
	json s1 = config["Resources"]["Mesh resources"][resourceNum]["File"];
	json s2 = config["Resources"]["Mesh resources"][resourceNum]["Exported file"];
	json s3 = config["Resources"]["Mesh resources"][resourceNum]["Name"];

	file = s1.get<std::string>();
	exported_file = s2.get<std::string>();
	name = s3.get<std::string>();
}