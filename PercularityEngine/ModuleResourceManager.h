#ifndef __MODULE_RESOURCEMANAGER_H__
#define __MODULE_RESOURCEMANAGER_H__

#include "ModuleTaskManager.h"
#include "PercularityResource.h"

struct ImVec4;

class ImportFileTask : public Task
{
public:
	ImportFileTask(const char* filePath, RESOURCE_TYPE type, bool force) : 
		filePath(filePath), type(type), force(force) {}

	RESOURCE_TYPE type = RESOURCE_TYPE::UNKNOWN;
	bool force = false;
	const char* filePath;
	void Execute() override;
};

// ---------------------------------------------------
class ModuleResourceManager : public Module
{
public:
	ModuleResourceManager(Application* app, bool start_enabled = true) : Module(app, start_enabled) {}
	virtual ~ModuleResourceManager() {}

	bool Start();
	bool CleanUp();
	void OnTaskFinished(Task* task);

	// Save & Load
	void Load(const nlohmann::json &config) {}
	void Save(nlohmann::json &config) {}
	void LoadResources(const json &scene_file);
	void SaveResources(json &scene_file);

	void ReceiveExternalFile(const char* new_file);
	uint ImportFile(const char* new_file, RESOURCE_TYPE type, bool force = false);
	Resource* CreateNewResource(RESOURCE_TYPE type, uint specific_uuid = 0);

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
	uint saved_res = 0;
	std::string selected = "";
	std::string currentFolder = "";
};

#endif // __MODULE_RESOURCEMANAGER_H__