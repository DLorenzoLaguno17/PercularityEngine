#include "Application.h"
#include "ModuleInput.h"
#include "ModuleGui.h"
#include "ModuleAudio.h"
#include "ModuleWindow.h"

ModuleGui::ModuleGui(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

// Destructor
ModuleGui::~ModuleGui()
{}

// Called before render is available
bool ModuleGui::Awake()
{
	LOG("Loading GUI atlas");
	bool ret = true;	

	/*beige = {};
	brown = {};
	grey = {};
	white = {};*/

	return ret;
}

// Called before the first frame
bool ModuleGui::Start()
{

	return true;
}

// Update all guis
bool ModuleGui::PreUpdate()
{
	
	return true;
}

// Called after all Updates
bool ModuleGui::PostUpdate()
{

	return true;
}

// Called before quitting
bool ModuleGui::CleanUp()
{
	LOG("Freeing Gui");	

	return true;
}