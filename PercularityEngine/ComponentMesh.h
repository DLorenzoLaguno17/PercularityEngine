#ifndef _ComponentMesh_H_
#define _ComponentMesh_H_

#include "Component.h"

#define NORMALS_LENGTH 1.0f

struct par_shapes_mesh_s;
class ResourceMesh;

class ComponentMesh : public Component {
public:
	ComponentMesh(GameObject* parent, bool active);

	~ComponentMesh() {}

	// Methods
	void Update() override;
	void OnEditor();
	void CleanUp() override;

	// Rendering methods
	void Render() const;
	void RenderNormals();

	// Load & Save 
	void OnLoad(const char* gameObjectNum, const nlohmann::json &scene_file);
	void OnSave(const char* gameObjectNum, nlohmann::json &scene_file);	

	// Create a primitive shape
	void LoadParShape(par_shapes_mesh_s* parShape);
	const AABB& GetAABB()const { return aabb; }

	static COMPONENT_TYPE GetComponentType() { return COMPONENT_TYPE::MESH; }

public:
	// Bounding box
	AABB aabb;

	ResourceMesh* resource_mesh = nullptr;

private:
	bool showVertexNormals = false;
	bool showFaceNormals = false;
};

#endif // _ComponentMesh_H_