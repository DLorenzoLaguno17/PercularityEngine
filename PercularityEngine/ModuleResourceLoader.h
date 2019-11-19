#ifndef __ModuleResourceLoader_H__
#define __ModuleResourceLoader_H__

#include "Module.h"

#define CHECKERS_WIDTH 150
#define CHECKERS_HEIGHT 150

class GameObject;
class ComponentMesh;
class ComponentMaterial;
struct aiMesh;

// ---------------------------------------------------
class ModuleResourceLoader : public Module
{
public:
	ModuleResourceLoader(Application* app, bool start_enabled = true) : Module(app, start_enabled) {}

	// Destructor
	virtual ~ModuleResourceLoader() {}

	// Called before render is available
	bool Init();

	// Call before first frame
	bool Start();

	// Called before quitting
	bool CleanUp();

	// Save & Load
	void Load(const nlohmann::json &config) {}
	void Save(nlohmann::json &config) {}

	// Loading methods
	void LoadModel(const char* path);
	void LoadTexture(const char* path, ComponentMaterial* material);
	void LoadMesh(ComponentMesh* c_mesh, aiMesh* currentMesh);

	// Importing methods 
	void ImportFile(const char* full_path);
	bool ImportTextureToLibrary(const char* path, std::string& output_file);
	bool ImportMeshToLibrary(const char* path, ComponentMesh* mesh);
	void LoadMeshFromLibrary(const char* path, ComponentMesh* mesh);
	
	// Useful methods
	void CreateDefaultTexture();
	bool CheckTextureExtension(const char* extension);
	bool CheckMeshExtension(const char* extension);
	std::string getNameFromPath(std::string path, bool withExtension = false);

public:
	ComponentMaterial* icon_tex = nullptr;
	uint default_tex = 0;
};

#endif // __ModuleResourceLoader_H__