#include "SettingsWindow.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleWindow.h"
#include "imgui.h"

#include "OpenGL.h"
#include "DevIL/include/il.h"
#include "SDL/include/SDL_opengl.h"

SettingsWindow::SettingsWindow(char* name, bool active) : UIElement(name, active) {}

// Show settings window
void SettingsWindow::Update() {
	
	if (!timerStarted) {
		dblcTimer.Start();
		timerStarted = true;
	}

	ImGui::Begin("Settings", &active);

	// General settings
	if (ImGui::CollapsingHeader("Style")) {

		ImGui::NewLine();
		ImGui::ShowFontSelector("Select font");
		ImGui::ShowStyleSelector("Select style");
		ImGui::NewLine();
	}

	// Window settings
	if (ImGui::CollapsingHeader("Window")) {
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

		static int w = SCREEN_WIDTH;
		static int h = SCREEN_HEIGHT;
		static float b = 1.0f;
		ImGui::NewLine();
		ImGui::SliderInt("Width", &w, SCREEN_WIDTH, 1920);
		ImGui::SliderInt("Height", &h, SCREEN_HEIGHT, 1080);
		ImGui::SliderFloat("Brightness", &b, 0.0f, 1.0f);
		SDL_SetWindowBrightness(App->window->window, b);
		SDL_SetWindowSize(App->window->window, w, h);

		ImGui::NewLine();
	}

	// Renderer settings
	if (ImGui::CollapsingHeader("Renderer")) {
		ImGui::NewLine();

		if (ImGui::Checkbox("Depth test", &depthTest)) {
			if (depthTest) glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Lighting", &lighting)) {
			if (lighting) glEnable(GL_LIGHTING);
			else glDisable(GL_LIGHTING);
		}
		if (ImGui::Checkbox("Texture 2D", &texture2D)) {
			if (texture2D) glEnable(GL_TEXTURE_2D);
			else glDisable(GL_TEXTURE_2D);
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Color material", &colorMaterial)) {
			if (colorMaterial) glEnable(GL_COLOR_MATERIAL);
			else glDisable(GL_COLOR_MATERIAL);
		}
		if (ImGui::Checkbox("Wireframe", &wireframe)) {
			if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Cull face", &cullFace)) {
			if (cullFace) glEnable(GL_CULL_FACE);
			else glDisable(GL_CULL_FACE);
		}

		ImGui::NewLine();
	}

	// Performance settings
	if (ImGui::CollapsingHeader("Performance")) {
		ImGui::NewLine();
		if (ImGui::Checkbox("V. Sync", &vsync)) App->DisableVsync(vsync);
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

		ImGui::NewLine();
		ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
		ImGui::Text("GPU: %s", glGetString(GL_RENDERER));
		SDL_version v; SDL_GetVersion(&v);
		ImGui::Text("SDL version: %d.%d.%d", v.major, v.minor, v.patch);
		ImGui::Text("OpenGL version supported: %s", glGetString(GL_VERSION));
		ImGui::Text("DevIL version: %d", ilGetInteger(IL_VERSION_NUM));
		ImGui::Text("Dear ImGui version: %s", ImGui::GetVersion());
		ImGui::Text("Glew version: %s", glewGetString(GLEW_VERSION)); 
		ImGui::Text("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION)); 
		ImGui::NewLine();
	}

	// Input read	
	if (ImGui::CollapsingHeader("Input")) {
		ImGui::NewLine();

		if (ImGui::IsMousePosValid())
			ImGui::Text("Mouse pos: (%g, %g)", App->gui->io->MousePos.x, App->gui->io->MousePos.y);
		else
			ImGui::Text("Mouse pos: <INVALID>");
		ImGui::Text("Mouse delta: (%g, %g)", App->gui->io->MouseDelta.x, App->gui->io->MouseDelta.y);

		ImGui::Text("Mouse down:");
		ImGui::SameLine();
		for (int i = 0; i < IM_ARRAYSIZE(App->gui->io->MouseDown); i++) {
			if (App->gui->io->MouseDownDuration[i] >= 0.0f) {
				showButtonInfo = false;
				ImGui::Text("Button %d (%.02f secs)", i, App->gui->io->MouseDownDuration[i]);
				secs = App->gui->io->MouseDownDuration[i];
				mouseButton = i;
			}

			if (ImGui::IsMouseReleased(i))
				showButtonInfo = true;
		}
		if (showButtonInfo) ImGui::Text("Button %d (%.02f secs)", mouseButton, secs);

		ImGui::Text("Double click:");
		ImGui::SameLine();

		for (int i = 0; i < IM_ARRAYSIZE(App->gui->io->MouseDown); i++) {
			if (ImGui::IsMouseDoubleClicked(i)) {
				showButtonInfo2 = true;
				lastTime = dblcTimer.Read();
				mouseButton2 = i;
			}
		}
		if (showButtonInfo2) ImGui::Text("Button %d", mouseButton2);

		if (showButtonInfo2 && dblcTimer.Read() > lastTime + 750) {
			showButtonInfo2 = false;
			lastTime = dblcTimer.Read();
		}
	}

	//App->gui->io->KeysDownDuration[];
	ImGui::End();
}