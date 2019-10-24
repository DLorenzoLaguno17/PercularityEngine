#ifndef _ComponentMesh_H_
#define _ComponentMesh_H_

#include "Component.h"
#include "imgui.h"
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
	const char* assigned_tex;

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
	void OnEditor() {
		if (ImGui::CollapsingHeader("Mesh")) {
			ImGui::Checkbox("Enabled", &active);

			ImGui::Text("Vertices:");
			ImGui::SameLine();
			uint cnt1 = 0;
			for (uint i = 0; i < mesh.size(); ++i) cnt1 += mesh[i]->num_vertices;
			ImGui::TextColored({ 255, 255, 0, 255 }, "%d", cnt1);

			ImGui::Text("Polygons:");
			ImGui::SameLine();
			uint cnt2 = 0;
			for (uint i = 0; i < mesh.size(); ++i) cnt2 += (mesh[i]->num_indices / 3);
			ImGui::TextColored({ 255, 255, 0, 255 }, "%d", cnt2);

			ImGui::NewLine();
		}
	}

public:
	std::vector<MeshData*> mesh;
};

#endif // _ComponentMesh_H_