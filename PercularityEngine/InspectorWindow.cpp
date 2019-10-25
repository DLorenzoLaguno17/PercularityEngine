#include "InspectorWindow.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"

#include <string>

#include "mmgr/mmgr.h"

InspectorWindow::InspectorWindow(char* name, bool active) : UIElement(name, active) {}

// Show scene window
void InspectorWindow::Update() {

	ImGui::Begin("Inspector", &active);

	if (App->scene->selected != nullptr) {
		ImGui::InputText("GameObject", (char*)App->scene->selected->name.c_str(), 20);

		ImGui::NewLine();
		App->scene->selected->OnEditor();
	}

	ImGui::End();
}