#include "HierarchyWindow.h"
#include "Application.h"

#include "mmgr/mmgr.h"

HierarchyWindow::HierarchyWindow(char* name, bool active) : UIElement(name, active) {}

// Show scene window
void HierarchyWindow::Update() {
	ImGui::Begin("Inspector", &active);


	ImGui::End();
}