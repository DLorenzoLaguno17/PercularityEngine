#ifndef _ComponentMesh_H_
#define _ComponentMesh_H_

#include "Component.h"
#include <vector>

struct MeshData {

	// Index 
	uint id_index = 0;
	uint num_indices = 0;
	uint* indices = nullptr;

	// Vertex
	uint id_vertex = 0;
	uint num_vertices = 0;
	float* vertices = nullptr;

	// Texture
	uint id_tex = 0;
	uint num_tex = 0;
	float* textures = nullptr;

	// Normals
	uint num_normals = 0;
	float* normals = nullptr;

	// Colors
	uint num_colors = 0;
	uint* colors = nullptr;

	// Delete the struct
	void CleanUp() {
		if (indices != nullptr) { delete indices; indices = nullptr; }
		if (vertices != nullptr) { delete vertices; vertices = nullptr; }
		if (textures != nullptr) { delete textures; textures = nullptr; }
		if (normals != nullptr) { delete normals; normals = nullptr; }
		if (colors != nullptr) { delete colors; colors = nullptr; }
	}
};

class ComponentMesh : public Component {
public:
	ComponentMesh(COMPONENT_TYPE type, GameObject* parent, bool active) :
		Component(type, parent, active) {}

	~ComponentMesh() {}

	// Methods
	void Update() {}
	void Enable() { active = true; }
	void Disable() { active = false; }

	void OnEditor() {}

	void Load() {};

public:
	std::vector<MeshData*> mesh;
};

#endif // _ComponentMesh_H_