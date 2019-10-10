#ifndef __ModuleMeshLoader_H__
#define __ModuleMeshLoader_H__

#include "Module.h"

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

public:
};

#endif // __ModuleMeshLoader_H__