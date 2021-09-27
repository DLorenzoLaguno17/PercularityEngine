#ifndef __MODULE_RESOURCEMANAGER_H__
#define __MODULE_RESOURCEMANAGER_H__

#include "Module.h"
#include "PercularityResource.h"

// ---------------------------------------------------
class ModuleResourceManager : public Module
{
public:
	ModuleResourceManager(Application* app, bool start_enabled = true) : Module(app, start_enabled) {}
	virtual ~ModuleResourceManager() {}

	bool Start();
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
	Resource* GetResourceByName(const char* name);
	uint FindFileInAssets(const char* existing_file) const;
	uint FindFileInLibrary(const char* exported_file) const;

	void DrawProjectExplorer();

private:
	// Extension checkers
	bool HasTextureExtension(const char* extension);
	bool HasModelExtension(const char* extension);
	RESOURCE_TYPE GetTypeFromExtension(const char* extension);

private:
	std::map<uint, Resource*> resources;
	//uint resourcesCount = 0;
	uint saved_res = 0;

};

#endif // __MODULE_RESOURCEMANAGER_H__