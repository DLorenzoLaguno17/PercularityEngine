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
{

}

// Save and Load
void ResourceTexture::OnSave(const char* resourceNum, json &config) const
{
	config["Resources"][resourceNum]["UUID"] = UUID;
	config["Resources"][resourceNum]["File"] = file;
	config["Resources"][resourceNum]["Exported file"] = exported_file;
	config["Resources"][resourceNum]["Name"] = name;
	config["Resources"][resourceNum]["Width"] = width;
	config["Resources"][resourceNum]["Height"] = height;
	config["Resources"][resourceNum]["Texture"] = texture;
	config["Resources"][resourceNum]["Type"] = "Texture";
}

void ResourceTexture::OnLoad(const char* resourceNum, const json &config)
{
	UUID = config["Resources"][resourceNum]["UUID"];
	width = config["Resources"][resourceNum]["Width"];
	height = config["Resources"][resourceNum]["Height"];
	texture = config["Resources"][resourceNum]["Texture"];
	json s1 = config["Resources"][resourceNum]["File"];
	json s2 = config["Resources"][resourceNum]["Exported file"];
	json s3 = config["Resources"][resourceNum]["Name"];

	file = s1.get<std::string>();
	exported_file = s2.get<std::string>();
	name = s3.get<std::string>();
}