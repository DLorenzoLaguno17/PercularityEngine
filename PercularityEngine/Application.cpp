#include "Application.h"
#include <fstream>
#include "gl3w.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	gui = new ModuleGui(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(gui);
	
	// Scenes

	// Renderer last!
	AddModule(renderer3D);

	//Configuration adress
	settingsAdress = "Configuration/.editorconfig.json";
}

Application::~Application()
{
	std::list<Module*>::iterator item = modules.begin();
	for (; item != modules.end(); item = next(item))
		delete(*item);

	modules.clear();
}

bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules
	std::list<Module*>::iterator item = modules.begin();
	gl3wInit();

	while(item != modules.end() && ret == true)
	{
		ret = (*item)->Init();
		item = next(item);
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
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
}

// ---------------------------------------------
void Application::FinishUpdate()
{
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
	std::list<Module*>::iterator item = modules.begin();


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

void Application::Load()
{
	json config;

	//Open the settings file so we can browse it
	std::ifstream fileOperator;
	fileOperator.open(settingsAdress);

	//Load configuration for all the modules
	std::list<Module*>::iterator it = modules.begin();
	
	config = json::parse(fileOperator);

	//close the file
	fileOperator.close();

	while (it != modules.end())
	{
		(*it)->Load(config);
		it++;
	}
}

void Application::Save()
{
	json config;

	//Save configuration for all the modules
	std::list<Module*>::iterator it = modules.begin();

	while (it != modules.end())
	{
		(*it)->Save(config);
		it++;
	}
}