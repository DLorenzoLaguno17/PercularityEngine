#ifndef __ResourceMesh_H__
#define __ResourceMesh_H__

#include "Resource.h"

class ResourceMesh : public Resource 
{
public:
	ResourceMesh(uint uuid) : Resource(uuid, RESOURCE_TYPE::MESH) {}

	virtual ~ResourceMesh() {}

	bool LoadInMemory();
	void ReleaseFromMemory();

	void OnSave(const char* resourceNum, json &config) const;
	void OnLoad(const char* resourceNum, const json &config);

public:
	// Index 
	uint id_index = 0;
	uint num_indices = 0;
	uint* indices = nullptr;

	// Vertex
	uint id_vertex = 0;
	uint num_vertices = 0;
	float3* vertices = nullptr;

	// Texture coordinates
	uint id_UVs = 0;
	uint num_UVs = 0;
	float* coords = nullptr;

	// Normals
	uint num_normals = 0;
	float3* normals = nullptr;

	// Colors
	uint num_colors = 0;
	uint* colors = nullptr;

};

#endif // __ResourceMesh_H__