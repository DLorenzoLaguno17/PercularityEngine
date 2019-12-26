#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics.h"
#include "ModuleGui.h"
#include "ModuleScene.h"
#include "ModuleResourceLoader.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "Time.h"

#include <fstream>
#include <iomanip>

#include "GLEW/include/glew.h"
#include "mmgr/mmgr.h"

Application::Application()
{
	// Creating the random number generator
	rand = new math::LCG();

	// Creating the modules
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	gui = new ModuleGui(this);
	scene = new ModuleScene(this);
	res_loader = new ModuleResourceLoader(this);
	res_manager = new ModuleResourceManager(this);
	file_system = new ModuleFileSystem(this, ASSETS_FOLDER);
	physics = new ModulePhysics(this);

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(gui);
	AddModule(file_system);
	AddModule(res_manager);
	AddModule(res_loader);
	AddModule(physics);
	
	// Scenes
	AddModule(scene);

	// Renderer last!
	AddModule(renderer3D);

	// Configuration adress
	settingsAddress = "Configuration/.editorconfig.json";
}

Application::~Application()
{
	std::list<Module*>::iterator item = modules.begin();
	for (; item != modules.end(); item = next(item))
		delete(*item);

	modules.clear();

	if (rand) RELEASE(rand);
}

bool Application::Init()
{
	bool ret = true;

	LoadSettings();

	// Call Init() in all modules
	std::list<Module*>::iterator item = modules.begin();
	glewInit();

	while(item != modules.end() && ret == true)
	{
		ret = (*item)->Init();
		item = next(item);
	}

	// After all Init calls we call Start() in all modules
	LOG("-------------- Application Start --------------");
	item = modules.begin();

	while(item != modules.end() && ret == true)
	{
		ret = (*item)->Start();
		item = next(item);
	} 
	
	ms_timer.Start();


	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
	Time::PreUpdate(dt);
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	Time::Update();
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	std::list<Module*>::iterator item = modules.begin();

	while(item != modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PreUpdate(dt);
		item = next(item);
	}

	item = modules.begin();

	while (item != modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->Update(dt);
		item = next(item);
	}

	item = modules.begin();

	while (item != modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PostUpdate(dt);
		item = next(item);
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	closingEngine = true;
	std::list<Module*>::iterator item = modules.begin();

	SaveSettings();

	while(item != modules.end() && ret == true)
	{
		ret = (*item)->CleanUp();
		item = next(item);
	}
	return ret;
}

void Application::AddModule(Module* mod)
{
	modules.push_back(mod);
}

void Application::LoadSettings()
{
	json config;

	//If the adress of the settings file is null, create  an exception
	assert(settingsAddress != nullptr);

	//Create a stream and open the file
	std::ifstream stream;
	stream.open(settingsAddress);

	//Load configuration for all the modules
	std::list<Module*>::iterator it = modules.begin();
	
	config = json::parse(stream);

	//close the file
	stream.close();

	std::string name = config["Application"]["Name"];
	engineName = name;

	std::string version = config["Application"]["Version"];
	engineVersion = version;
	
	while (it != modules.end())
	{
		(*it)->Load(config);
		it++;
	}

	App->gui->settings->vsync = config["Application"]["VSYNC"];
	DisableVsync(!App->gui->settings->vsync);
	App->gui->settings->fulldesktop = App->window->winFullscreenDesktop;
}

void Application::SaveSettings()
{
	//Create auxiliar file
	json config;
	
	config["Application"]["Name"] = engineName;
	config["Application"]["Version"] = engineVersion;
	config["Application"]["VSYNC"] = App->gui->settings->vsync;

	//Save configuration for all the modules
	std::list<Module*>::iterator it = modules.begin();

	while (it != modules.end())
	{
		(*it)->Save(config);
		it++;
	}

	//Create the stream and open the file
	std::ofstream stream;
	stream.open(settingsAddress);
	stream << std::setw(4) << config << std::endl;
	stream.close();
}

void Application::DisableVsync(bool mustDisable) {
	if (mustDisable)
		SDL_GL_SetSwapInterval(1);
	else 
		SDL_GL_SetSwapInterval(0);
}

LCG& Application::GetRandomGenerator() {
	return *rand;
}