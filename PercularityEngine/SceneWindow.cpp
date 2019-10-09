#include "SceneWindow.h"
#include "Application.h"
#include "ModuleScene.h"
#include "imgui.h"

SceneWindow::SceneWindow(char* name, bool active) : UIElement(name, active){}

// Show scene window
void SceneWindow::Update() {

	ImGui::Begin("Scene", &active);
	
	ImGui::Image((void*)(intptr_t)App->scene->texColorBuffer, ImVec2(ImGui::GetContentRegionMax().x, ImGui::GetContentRegionMax().y), ImVec2(0, 1), ImVec2(1, 0));

	ImGui::End();
}