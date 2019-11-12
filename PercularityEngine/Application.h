#pragma once

#include "Globals.h"
#include "Timer.h"
#include "Module.h"

#include "Json Parser/nlohmann/json.hpp"

#include <vector>
#include <string>
#include <list>

class ModuleWindow;
class ModuleInput;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleGui;
class ModuleScene;
class ModuleResourceLoader;
class ModuleFileSystem;

using json = nlohmann::json;

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleGui* gui;
	ModuleResourceLoader* res_loader;
	ModuleFileSystem* file_system;
	ModuleScene* scene;

	//Aditional data
	std::string engineName;
	std::string engineVersion;

private:

	Timer	ms_timer;
	float	dt;
	std::list<Module*> modules;

	//Save & load
	const char* settingsAdress;
	json		settingsFile;	
	
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();
	void DisableVsync(bool mustDisable);

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

	//Save & load
	void LoadSettings();	//Called before modules Init() & Start()
	void SaveSettings();
};

extern Application* App;
extern std::vector<std::string> log_list;