#include "ConsoleWindow.h"
#include "Application.h"

#include "mmgr/mmgr.h"

ConsoleWindow::ConsoleWindow(char* name, bool active) : UIElement(name, active) {}

// Show scene window
void ConsoleWindow::Update() {
	ImGui::Begin("Console", &active);

	for (int i = 0; i < log_list.size(); ++i) {
		ImGui::Text(log_list[i].c_str()); 		
		//ImGui::SetScrollHereY(1.0f);
	}

	ImGui::End();
}