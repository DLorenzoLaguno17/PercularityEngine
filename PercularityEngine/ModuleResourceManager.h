#ifndef __ModuleResourceManager_H__
#define __ModuleResourceManager_H__

#include "Module.h"
#include "Resource.h"

// ---------------------------------------------------
class ModuleResourceManager : public Module
{
public:
	ModuleResourceManager(Application* app, bool start_enabled = true) : Module(app, start_enabled) {}

	// Destructor
	virtual ~ModuleResourceManager() {}

	// Call before first frame
	bool Start();

	// Called before quitting
	bool CleanUp();

	// Save & Load
	void Load(const nlohmann::json &config) {}
	void Save(nlohmann::json &config) {}
	void LoadResources(const json &scene_file);
	void SaveResources(json &scene_file);

	uint ReceiveExternalFile(const char* new_file);
	Resource* CreateNewResource(RESOURCE_TYPE type, uint specific_uuid = 0);
	uint ImportFile(const char* new_file, RESOURCE_TYPE type, bool force = false);

	// Getters
	const Resource* GetResourceFromMap(uint uuid) const;
	Resource* GetResourceFromMap(uint uuid);
	uint FindFileInAssets(const char* existing_file) const;

	// Extension checkers
	bool CheckTextureExtension(const char* extension);
	bool CheckModelExtension(const char* extension);
	RESOURCE_TYPE GetTypeFromExtension(const char* extension);

	void DrawProjectExplorer();

private:
	std::map<uint, Resource*> resources;
	uint resourcesCount = 0;
	uint saved_res = 0;

};

#endif // __ModuleResourceManager_H__