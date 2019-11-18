#ifndef _ComponentMesh_H_
#define _ComponentMesh_H_

#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"

#define NORMALS_LENGTH 1.0f

struct par_shapes_mesh_s;
class ResourceMesh;

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
	ComponentMesh(GameObject* parent, bool active);

	~ComponentMesh() {}

	// Methods
	void Update() override;
	void OnEditor();
	void CleanUp()override;

	// Load & Save 
	void OnLoad(const char* gameObjectNum, const nlohmann::json &scene_file);
	void OnSave(const char* gameObjectNum, nlohmann::json &scene_file);

	// Rendering methods
	void Render() const ;
	void RenderNormals();

	//Create a primitive shape
	void LoadParShape(par_shapes_mesh_s* parShape);
	
	const AABB& GetAABB()const { return aabb; }

	static COMPONENT_TYPE GetComponentType() { return COMPONENT_TYPE::MESH; }

public:
	MeshData mesh;
	AABB aabb;
	class ResourceMesh* resource_mesh = nullptr;

private:
	bool showVertexNormals = false;
	bool showFaceNormals = false;
};

#endif // _ComponentMesh_H_