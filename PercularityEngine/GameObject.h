#ifndef __GameObject_H__
#define __GameObject_H__

#include "Component.h"
//#include "MathGeoLib/include/MathGeoLib.h"
#include <vector>

#define NORMALS_LENGTH 1.0f

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

class GameObject {
public:
	//Constructors
	GameObject() {};
	GameObject(char* name, bool active, GameObject* parent = nullptr) : 
		name(name), active(active), parent(parent) {};

	// Destructor
	virtual ~GameObject() {};

	// Rendering methods
	void Render() const;
	void ShowNormals();

	// Called every frame
	void Update();

	// Removes the memory
	void CleanUp();

	void CreateComponent(COMPONENT_TYPE type);

public:
	// List of data
	std::vector<MeshData*> mesh;
	// Texture of the GameObject
	uint texture = 0;
	// Transform
	//float4x4 transform;

	char* name;
	bool active;
	std::vector<Component*> components;
	std::vector<GameObject*> children;
	GameObject* parent;
};

#endif // __GameObject_H__