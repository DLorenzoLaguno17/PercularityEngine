#include "ResourceMesh.h"
#include "Application.h"
#include "ModuleResourceLoader.h"

bool ResourceMesh::LoadInMemory() 
{ 
	return App->res_loader->LoadMeshFromLibrary(exported_file.c_str(), this);
}

void ResourceMesh::ReleaseFromMemory() 
{
	RELEASE_ARRAY(indices);
	RELEASE_ARRAY(vertices);
	RELEASE_ARRAY(normals);
	RELEASE_ARRAY(colors);
	RELEASE_ARRAY(coords);
}

// Save and Load
void ResourceMesh::OnSave(const char* resourceNum, nlohmann::json &config) const
{
	config["Resources"]["Texture resources"][resourceNum]["UUID"] = UUID;
	config["Resources"]["Texture resources"][resourceNum]["File"] = file;
	config["Resources"]["Texture resources"][resourceNum]["Exported file"] = exported_file;
	config["Resources"]["Texture resources"][resourceNum]["Name"] = name;
}

void ResourceMesh::OnLoad(const char* resourceNum, const nlohmann::json &config)
{
	UUID = config["Resources"]["Texture resources"][resourceNum]["UUID"];
	json s1 = config["Resources"]["Texture resources"][resourceNum]["Name"];
	json s2 = config["Resources"]["Texture resources"][resourceNum]["File"];
	json s3 = config["Resources"]["Texture resources"][resourceNum]["Exported file"];

	name = s1.get<std::string>();
	file = s2.get<std::string>();
	exported_file = s3.get<std::string>();

	aabb.SetNegativeInfinity();
	aabb = AABB::MinimalEnclosingAABB(vertices, num_vertices);
}