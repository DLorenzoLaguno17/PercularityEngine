#pragma once

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include <list>

#include "MathGeoLib/include/Algorithm/Random/LCG.h"

class ModuleWindow;
class ModuleInput;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleGui;
class ModuleScene;
class ModuleResourceLoader;
class ModuleResourceManager;
class ModuleFileSystem;
class ModulePhysics;
class ModulePlayer;
class ModuleUndo;
class ModuleTaskManager;

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleGui* gui;
	ModuleResourceLoader* res_loader;
	ModuleResourceManager* res_manager;
	ModuleFileSystem* file_system;
	ModuleScene* scene;
	ModulePhysics* physics;
	ModulePlayer* player;
	ModuleUndo* undo;
	ModuleTaskManager* task_manager;

	// Aditional data
	std::string engineName;
	std::string engineVersion;

	bool closingEngine = false;

private:
	Timer	ms_timer;
	float	dt;
	std::list<Module*> modules;

	//Save & load
	const char* settingsAddress;
	json		settingsFile;

	// Random number generator
	math::LCG* rand = nullptr;
	
public:
	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();
	void DisableVsync(bool mustDisable);
	LCG& GetRandomGenerator();

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