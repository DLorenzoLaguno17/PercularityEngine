#ifndef __ModuleResourceLoader_H__
#define __ModuleResourceLoader_H__

#include "Module.h"
#include <vector>

#define CHECKERS_WIDTH 150
#define CHECKERS_HEIGHT 150

class GameObject;
class ComponentMesh;
class ComponentMaterial;
class aiMesh;

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
	void LoadMesh(const char* path, ComponentMesh* c_mesh, aiMesh* currentMesh);

	// Importing methods 
	void ImportFile(const char* full_path);
	bool ImportTexture(const char* path, std::string& output_file);
	bool ImportMesh(ComponentMesh* mesh, std::string& output_file);
	void LoadMeshFromLibrary(ComponentMesh* mesh, char* buffer);
	
	// Useful methods
	void CreateDefaultTexture();
	std::string getNameFromPath(std::string path, bool withExtension = false);
	bool CheckTextureExtension(const char* extension);
	bool CheckMeshExtension(const char* extension);

public:
	ComponentMaterial* icon_tex = nullptr;
	uint default_tex = 0;
};

#endif // __ModuleResourceLoader_H__