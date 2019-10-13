#ifndef __ModuleResourceLoader_H__
#define __ModuleResourceLoader_H__

#include "Module.h"
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
};

struct GameObject {

	// List of data
	std::vector<MeshData*> mesh;
	// Texture of the GameObject
	uint texture = 0;
};

// ---------------------------------------------------
class ModuleResourceLoader : public Module
{
public:

	ModuleResourceLoader(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleResourceLoader();

	// Called when before render is available
	bool Init();

	// Call before first frame
	bool Start();

	// Called before all Updates
	update_status PreUpdate(float dt);

	// Called every frame
	update_status Update(float dt);

	// Called after all Updates
	update_status PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	// Save & Load
	void Load(const nlohmann::json  &config);
	void Save(nlohmann::json &config);

	// Methods to load and draw a mesh
	void LoadFBX(const char* path, uint tex = 0);
	void RenderFBX(GameObject fbx_mesh);
	void RenderNormals(GameObject fbx_mesh);

	// Method to create a texture
	uint CreateTexture(const char* path);

public:
	std::vector<GameObject> game_objects;
	uint default_tex;
	uint icon_tex;

private:
	uint demon_tex;

};

#endif // __ModuleResourceLoader_H__