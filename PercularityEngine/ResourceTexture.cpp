#include "ResourceTexture.h"
#include "Application.h"
#include "ModuleResourceLoader.h"

bool ResourceTexture::LoadInMemory()
{
	if (strcmp(file.c_str(), "None") != 0)		
		return App->res_loader->LoadTextureFromLibrary(this);
	else
		return false;
}

void ResourceTexture::ReleaseFromMemory()
{}

// Save and Load
void ResourceTexture::OnSave(const char* resourceNum, json &config) const
{
	config["Resources"]["Texture resources"][resourceNum]["UUID"] = UUID;
	config["Resources"]["Texture resources"][resourceNum]["File"] = file;
	config["Resources"]["Texture resources"][resourceNum]["Exported file"] = exported_file;
	config["Resources"]["Texture resources"][resourceNum]["Name"] = name;
	config["Resources"]["Texture resources"][resourceNum]["Width"] = width;
	config["Resources"]["Texture resources"][resourceNum]["Height"] = height;
	config["Resources"]["Texture resources"][resourceNum]["Texture"] = texture;
}

void ResourceTexture::OnLoad(const char* resourceNum, const json &config)
{
	UUID = config["Resources"]["Texture resources"][resourceNum]["UUID"];
	width = config["Resources"]["Texture resources"][resourceNum]["Width"];
	height = config["Resources"]["Texture resources"][resourceNum]["Height"];
	texture = config["Resources"]["Texture resources"][resourceNum]["Texture"];
	json s1 = config["Resources"]["Texture resources"][resourceNum]["File"];
	json s2 = config["Resources"]["Texture resources"][resourceNum]["Exported file"];
	json s3 = config["Resources"]["Texture resources"][resourceNum]["Name"];

	file = s1.get<std::string>();
	exported_file = s2.get<std::string>();
	name = s3.get<std::string>();
}