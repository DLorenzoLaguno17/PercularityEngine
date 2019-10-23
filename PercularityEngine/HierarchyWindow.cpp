#include "HierarchyWindow.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"

#include "mmgr/mmgr.h"

HierarchyWindow::HierarchyWindow(char* name, bool active) : UIElement(name, active) {}

// Show scene window
void HierarchyWindow::Update() {
	ImGui::Begin("Hierarchy", &active);
	
	static int selected = -1;
	for (int i = 0; i < App->scene->game_objects.size(); ++i) {

		if (ImGui::Selectable(App->scene->game_objects[i].name, selected == i)) {
			selected = i;
			App->scene->selected = &App->scene->game_objects[i];
		}
	}

	ImGui::End();
}