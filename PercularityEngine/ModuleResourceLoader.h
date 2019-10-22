#ifndef __ModuleResourceLoader_H__
#define __ModuleResourceLoader_H__

#include "Module.h"
#include <vector>

#define NORMALS_LENGTH 1.0f

class GameObject;

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
	void Load(const nlohmann::json &config);
	void Save(nlohmann::json &config);

	// Methods to load
	void LoadFBX(const char* path, uint tex = 0);
	uint CreateTexture(const char* path);

	// Methods to enable/disable normals
	void EnableNormals();
	void DisableNormals();

	GameObject* CreateGameObject();

public:
	std::vector<GameObject> game_objects;
	uint default_tex;
	uint icon_tex;

private:
	uint demon_tex;
	uint house_tex;
	bool normalsShown = true;

	GameObject* root = nullptr;
};

#endif // __ModuleResourceLoader_H__