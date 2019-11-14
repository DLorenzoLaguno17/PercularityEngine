#ifndef _ComponentMesh_H_
#define _ComponentMesh_H_

#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"

#define NORMALS_LENGTH 1.0f

struct par_shapes_mesh_s;

struct MeshData {

	// Index 
	uint id_index = 0;
	uint num_indices = 0;
	uint* indices = nullptr;

	// Vertex
	uint id_vertex = 0;
	uint num_vertices = 0;
	float3* vertices = nullptr;

	// Texture coordinates
	uint id_tex = 0;
	uint num_tex = 0;
	float* textures = nullptr;

	// Normals
	uint num_normals = 0;
	float3* normals = nullptr;

	// Colors
	uint num_colors = 0;
	uint* colors = nullptr;

	// Delete the struct
	void CleanUp();
};

class ComponentMesh : public Component {
public:
	ComponentMesh(COMPONENT_TYPE type, GameObject* parent, bool active) :
		Component(type, parent, active) {}

	~ComponentMesh() {}

	// Methods
	void Update() override;
	void OnEditor();
	void CleanUp()override;

	// Rendering methods
	void Render() const ;
	void RenderNormals();

	//Create a primitive shape
	void LoadParShape(par_shapes_mesh_s* parShape);

	//Create the bouding box
	void CreateBoundingBox();

	static COMPONENT_TYPE GetComponentType()  { return COMPONENT_TYPE::MESH; }

public:
	MeshData mesh;

private:
	bool showVertexNormals = false;
	bool showFaceNormals = false;
};

#endif // _ComponentMesh_H_