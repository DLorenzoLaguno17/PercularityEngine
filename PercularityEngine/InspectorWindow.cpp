#include "InspectorWindow.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "imgui.h"

#include "mmgr/mmgr.h"

InspectorWindow::InspectorWindow(char* name, bool active) : UIElement(name, active) {}

// Show scene window
void InspectorWindow::Update() {

	ImGui::Begin("Inspector", &active);

	if (App->scene->selected != nullptr) {
		ImGui::TextColored(ImVec4(0, 128, 128, 255), App->scene->selected->name);
		ImGui::NewLine();
		App->scene->selected->OnEditor();
	}

	ImGui::End();
}