#ifndef __ModuleResourceManager_H__
#define __ModuleResourceManager_H__

#include "Module.h"

// ---------------------------------------------------
class ModuleResourceManager : public Module
{
public:
	ModuleResourceManager(Application* app, bool start_enabled = true) : Module(app, start_enabled) {}

	// Destructor
	virtual ~ModuleResourceManager() {}

	// Called before render is available
	bool Init();

	// Call before first frame
	bool Start();

	// Called before quitting
	bool CleanUp();

	// Save & Load
	void Load(const nlohmann::json &config) {}
	void Save(nlohmann::json &config) {}

public:
};

#endif // __ModuleResourceManager_H__