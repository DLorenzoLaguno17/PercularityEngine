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

	UID Find(const char* file_in_assets) const;
	UID ImportFile(const char* new_file_in_assets, RESOURCE_TYPE type, bool force = false);
	UID GenerateNewUID();
	const Resource* Get(UID uid) const;
	Resource* Get(UID uid);
	Resource* CreateNewResource(RESOURCE_TYPE type, UID force_uid = 0);

private:
	std::map<UID, Resource*> resources;

};

#endif // __ModuleResourceManager_H__