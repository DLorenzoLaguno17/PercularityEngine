#include "MainMenuBar.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleResourceLoader.h"
#include "ModuleGui.h"
#include "UIElement.h"
#include "ModuleScene.h"

#include "mmgr/mmgr.h"

// Show main menu bar
void MainMenuBar::Update(std::vector<UIElement*> list) {
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("Open");
			ImGui::MenuItem("Open recent", "Ctrl+0");
			ImGui::MenuItem("New");
			ImGui::Separator();
			ImGui::MenuItem("Save", " Ctrl+S");
			ImGui::MenuItem("Save as...", " Ctrl+Shift+S");
			ImGui::Separator();
			if (ImGui::MenuItem("Exit", "ESC"))
				App->renderer3D->status = UPDATE_STOP;

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::MenuItem("Undo", "Ctrl+Z");
			ImGui::MenuItem("Redo", "Ctrl+Y");

			ImGui::Separator();
			ImGui::MenuItem("Cut", "Ctrl+X");
			ImGui::MenuItem("Copy", "Ctrl+C");
			ImGui::MenuItem("Paste", "Ctrl+V");

			ImGui::Separator();
			ImGui::MenuItem("Duplicate", "Ctrl+D");
			ImGui::MenuItem("Delete", "Supr");

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("GameObjects"))
		{
			if (ImGui::BeginMenu("GameObjects"))
			{
				if (ImGui::MenuItem("Plane"))
					App->scene->CreatePlane(20.0f, 20.0f);

				if (ImGui::MenuItem("Sphere"))
					App->scene->CreateSphere(10,10,5);

				if (ImGui::MenuItem("Cube"))
					App->scene->CreateCube(10, 10, 10);

				if (ImGui::MenuItem("Donut"))
					App->scene->CreateDonut(10, 10, 5);

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Windows"))
		{
			for(int i = 0; i < list.size(); ++i)
				ImGui::MenuItem(list[i]->name, NULL, &list[i]->active);

			//ImGui::MenuItem("Demo window", NULL, &App->gui->show_demo_window);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Report a bug"))
				ShellExecuteA(NULL, "open", "https://github.com/DLorenzoLaguno17/PercularityEngine/issues/new/choose", NULL, NULL, SW_SHOWNORMAL);
			if (ImGui::MenuItem("Download the latest version"))
				ShellExecuteA(NULL, "open", "https://github.com/DLorenzoLaguno17/PercularityEngine/releases", NULL, NULL, SW_SHOWNORMAL);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("About"))
		{
			ImGui::Image((void*)App->res_loader->icon_tex, ImVec2(20, 20));
			ImGui::SameLine();
			ImGui::Text("Percularity v0.1");
			ImGui::Text("3D engine developed for student purposes");
			ImGui::Text("By Joan Marin & Daniel Lorenzo");
			if (ImGui::Button("Go to our GitHub"))
				ShellExecuteA(NULL, "open", "https://github.com/DLorenzoLaguno17/PercularityEngine", NULL, NULL, SW_SHOWNORMAL);
			ImGui::NewLine();

			ImGui::Separator();
			ImGui::Text("3rd party libraries used:");
			ImGui::BulletText("SDL 2.0.10");
			ImGui::BulletText("STL 2.0");
			ImGui::BulletText("Dear ImGui 1.72b");
			ImGui::BulletText("MathGeoLib 1.5");
			ImGui::BulletText("Open GL 4.6");
			ImGui::BulletText("Glew 2.1");
			ImGui::BulletText("Assimp 4.1");
			ImGui::BulletText("DevIL 1.8");
			ImGui::NewLine();

			ImGui::Separator();
			ImGui::Text("MIT License");
			ImGui::NewLine();
			ImGui::Text("Copyright(c) 2019 Joan Marin & Dani Lorenzo");
			ImGui::NewLine();
			ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy");
			ImGui::Text("of this software and associated documentation files (the 'Software'), to deal");
			ImGui::Text("in the Software without restriction, including without limitation the rights");
			ImGui::Text("to use, copy, modify, merge, publish, distribute, sublicense, and/or sell");
			ImGui::Text("copies of the Software, and to permit persons to whom the Software is");
			ImGui::Text("furnished to do so, subject to the following conditions:");
			ImGui::NewLine();
			ImGui::Text("The above copyright notice and this permission notice shall be included in all");
			ImGui::Text("copies or substantial portions of the Software.");
			ImGui::NewLine();
			ImGui::Text("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR");
			ImGui::Text("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,");
			ImGui::Text("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE");
			ImGui::Text("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER");
			ImGui::Text("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,");
			ImGui::Text("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE");
			ImGui::Text("SOFTWARE.");

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}