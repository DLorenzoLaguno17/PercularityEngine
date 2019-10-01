#include "SettingsWindow.h"
#include "ModuleGui.h"
#include "SDL/include/SDL_opengl.h"
#include "Application.h"

// Show settings window
void SettingsWindow::Update(float dt, Application* App) {
	
	static float f = 0.0f;
	ImGui::Begin("Settings");

	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
	ImGui::ColorEdit3("Background color", (float*)&clear_color);

	ImGui::NewLine();
	if (ImGui::Checkbox("Fullscreen", &fullscreen)) {
		App->window->SetFullscreen(fullscreen);
		resizable = false;
		borderless = false;
		fulldesktop = false;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Resizable", &resizable)) {
		App->window->SetResizable(resizable);
		fullscreen = false;
		borderless = false;
		fulldesktop = false;
	}
	if (ImGui::Checkbox("Borderless", &borderless)) {
		App->window->SetBorderless(borderless);
		resizable = false;
		fullscreen = false;
		fulldesktop = false;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Full desktop", &fulldesktop)) {
		App->window->SetFulldesktop(fulldesktop);
		resizable = false;
		borderless = false;
		fullscreen = false;
	}

	ImGui::NewLine();
	ImGui::ShowFontSelector("Select font");
	ImGui::ShowStyleSelector("Select style");

	ImGui::NewLine();
	ImGui::Separator();
	if (ImGui::Checkbox("Depth test", &depthTest)) {
		if (depthTest) glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Cull face", &cullFace)) {
		if (cullFace) glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
	}
	if (ImGui::Checkbox("Lighting", &lighting)) {
		if (lighting) glEnable(GL_LIGHTING);
		else glDisable(GL_LIGHTING);
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Color material", &colorMaterial)) {
		if (colorMaterial) glEnable(GL_COLOR_MATERIAL);
		else glDisable(GL_COLOR_MATERIAL);
	}

	ImGui::NewLine();
	ImGui::Separator();
	ImGui::Checkbox("V. Sync", &vsync);
	ImGui::NewLine();

	char title[25];

	fps.push_back(App->gui->io->Framerate);
	if (fps.size() > 100) {
		fps.erase(fps.begin());
	}
	sprintf_s(title, 25, "Framerate %.1f", fps[fps.size() - 1]);
	ImGui::PlotHistogram("##framerate", &fps[0], fps.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));

	ms.push_back(1000 / App->gui->io->Framerate);
	if (ms.size() > 100) {
		ms.erase(ms.begin());
	}
	sprintf_s(title, 25, "Milliseconds %.3f", ms[ms.size() - 1]);
	ImGui::PlotHistogram("##milliseconds", &ms[0], ms.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));

	ImGui::End();	
}

bool SettingsWindow::CleanUp() {

	return true;
}