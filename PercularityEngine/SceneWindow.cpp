#include "SceneWindow.h"
#include "imgui.h"

SceneWindow::SceneWindow(char* name, bool active) : UIElement(name, active){}

// Show scene window
void SceneWindow::Update() {
	ImGui::Begin("Scene", &active);

	ImGui::End();
}