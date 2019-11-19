#include "ProjectWindow.h"
#include "Application.h"

#include "mmgr/mmgr.h"

ProjectWindow::ProjectWindow(char* name, bool active) : UIElement(name, active) {}

// Show the project explorer
void ProjectWindow::Update() {
	ImGui::Begin("Project", &active);

	ImGui::End();
}