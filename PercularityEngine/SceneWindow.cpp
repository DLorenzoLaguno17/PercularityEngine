#include "SceneWindow.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ComponentCamera.h"

#include "mmgr/mmgr.h"

SceneWindow::SceneWindow(char* name, bool active) : UIElement(name, active) {}

// Show scene window
void SceneWindow::Update() {

	ImGui::Begin("Scene", &active, ImGuiWindowFlags_NoScrollbar| ImGuiWindowFlags_NoScrollWithMouse);

	windowPosition.x = ImGui::GetWindowPos().x;
	windowPosition.y = ImGui::GetWindowPos().y;
	windowSize.x = ImGui::GetContentRegionAvail().x;
	windowSize.y = ImGui::GetContentRegionAvail().y;

	// Check if the scene window has been resized
	if (windowSize.x != last_windowSize.x || windowSize.y != last_windowSize.y)
	{
		last_windowSize = windowSize;
	}
	
	// Scene image
	if (windowSize.x > windowSize.y)
		ImGui::Image((void*)App->renderer3D->GetTexColorBuffer(), ImVec2(windowSize.x , windowSize.x*App->renderer3D->GetCamera()->GetAspectRatio()),ImVec2(0, 1), ImVec2(1, 0));
	else 
		ImGui::Image((void*)App->renderer3D->GetTexColorBuffer(), ImVec2(windowSize.y/ App->renderer3D->GetCamera()->GetAspectRatio(), windowSize.y), ImVec2(0, 1), ImVec2(1, 0));
	
	ImGui::End();
}