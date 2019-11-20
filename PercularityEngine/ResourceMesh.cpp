#include "ResourceMesh.h"
#include "Application.h"
#include "ModuleResourceLoader.h"

bool ResourceMesh::LoadInMemory() 
{ 
	return App->res_loader->LoadMeshFromLibrary(exported_file.c_str(), this);
}

void ResourceMesh::ReleaseFromMemory() {
	CleanUp();
}

void ResourceMesh::CleanUp() 
{
	RELEASE_ARRAY(indices);
	RELEASE_ARRAY(vertices);
	RELEASE_ARRAY(normals);
	RELEASE_ARRAY(colors);
	RELEASE_ARRAY(coords);
}