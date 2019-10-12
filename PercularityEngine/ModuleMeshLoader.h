#ifndef __ModuleMeshLoader_H__
#define __ModuleMeshLoader_H__

#include "Module.h"
#include <vector>

#define CHECKERS_WIDTH 10
#define CHECKERS_HEIGHT 10

struct MeshData {

	uint id_index = 0; // Index in VRAM
	uint num_indices = 0;
	uint* indices = nullptr;

	uint id_vertex = 0; // Unique vertex in VRAM
	uint num_vertices = 0;
	float* vertices = nullptr;
};

typedef std::vector<MeshData*> FBX_Mesh;

// ---------------------------------------------------
class ModuleMeshLoader : public Module
{
public:

	ModuleMeshLoader(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleMeshLoader();

	// Called when before render is available
	bool Awake();

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

	//Save & Load
	void Load(const nlohmann::json  &config);
	void Save(nlohmann::json &config);

	//Methods to load and draw a mesh
	void LoadFBX(const char* path);
	void RenderFBX(FBX_Mesh mesh);

	//Method to create a texture
	void CreateTexture();

public:
	std::vector<FBX_Mesh> FBX_list;

private:
	uint test_tex_buffer = 0;
};

#endif // __ModuleMeshLoader_H__