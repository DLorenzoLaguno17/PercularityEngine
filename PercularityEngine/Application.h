#pragma once

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleGui.h"

#include "Json Parser/nlohmann/json.hpp"

#include <vector>
#include <string>
#include <list>

using json = nlohmann::json;

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleGui* gui;

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

public:

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