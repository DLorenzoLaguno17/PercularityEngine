#ifndef __ModuleResourceLoader_H__
#define __ModuleResourceLoader_H__

#include "Module.h"
#include <vector>

#define CHECKERS_WIDTH 150
#define CHECKERS_HEIGHT 150

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

	// Called before quitting
	bool CleanUp();

	// Save & Load
	void Load(const nlohmann::json &config);
	void Save(nlohmann::json &config);

	// Methods to load
	void LoadFBX(const char* path, uint tex = 0);
	void CreateDefaultTexture();
	uint CreateTexture(const char* path, long* width = nullptr, long* height = nullptr);
	std::string getNameFromPath(std::string path, bool withExtension = false);

public:
	uint icon_tex = 0;
	uint default_tex = 0;

private:
	bool loadedAll = false;
};

#endif // __ModuleResourceLoader_H__