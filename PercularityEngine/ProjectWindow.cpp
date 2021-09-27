#include "ProjectWindow.h"
#include "Application.h"
#include "ModuleResourceManager.h"

#include "mmgr/mmgr.h"

ProjectWindow::ProjectWindow(char* name, bool active) : UIElement(name, active) {}

// Show the project explorer
void ProjectWindow::Update() 
{
	ImGui::Begin("Project", &active);
	App->res_manager->DrawProjectExplorer();
	ImGui::End();
}