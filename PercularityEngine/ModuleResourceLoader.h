#ifndef __ModuleResourceLoader_H__
#define __ModuleResourceLoader_H__

#include "Module.h"
#include <vector>

struct MeshData {

	uint id_index = 0; // Index in VRAM
	uint num_indices = 0;
	uint* indices = nullptr;

	uint id_vertex = 0; // Vertex in VRAM
	uint num_vertices = 0;
	float* vertices = nullptr;

	uint id_tex = 0;  // Texture in VRAM
	uint num_tex = 0;
	float* textures = nullptr;
};

struct FBX_Mesh {

	// List of data
	std::vector<MeshData*> mesh;
	// Texture of the FBX
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
	void LoadFBX(const char* path, uint tex);
	void RenderFBX(FBX_Mesh fbx_mesh);

	// Method to create a texture
	uint CreateTexture(const char* path);

public:
	std::vector<FBX_Mesh> FBX_list;
	uint default_tex = 0;

};

#endif // __ModuleResourceLoader_H__