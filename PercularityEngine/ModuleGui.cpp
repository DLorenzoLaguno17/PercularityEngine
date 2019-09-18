#include "Application.h"
#include "ModuleInput.h"
#include "ModuleGui.h"
#include "ModuleAudio.h"
#include "ModuleWindow.h"
#include "imgui.h"

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

	beige = { 245, 245, 220, 255 };
	brown = { 73, 31, 10, 255 };
	grey = { 134, 136, 138, 255 };
	white = { 255, 255, 255, 255 };

	return ret;
}

// Called before the first frame
bool ModuleGui::Start()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

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