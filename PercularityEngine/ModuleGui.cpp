#include "Application.h"
#include "ModuleInput.h"
#include "ModuleGui.h"
#include "ModuleWindow.h"

#include "GLEW/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "Brofiler/Lib/Brofiler.h"

ModuleGui::ModuleGui(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

// Destructor
ModuleGui::~ModuleGui()
{}

// Called before render is available
bool ModuleGui::Awake()
{
	LOG("Loading GUI atlas");
	bool ret = true;	

	return ret;
}

// Called before the first frame
bool ModuleGui::Start()
{
	IMGUI_CHECKVERSION();

	// Setting context
	ImGui::CreateContext();
	SDL_GL_MakeCurrent(App->window->window, App->renderer3D->context);
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);

	// Setting GuiIO
	io = &ImGui::GetIO(); (void)io;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	// Create window with graphics context
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);	

#if defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
	bool err = glewInit() != 0;
#endif
	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}
	else {
		LOG("Using Glew %s", glewGetString(GLEW_VERSION)); // Sould be 2.0

		LOG("Vendor %s", glGetString(GL_VENDOR));
		LOG("Renderer: %s", glGetString(GL_RENDERER));
		LOG("OpenGL version supported: %s", glGetString(GL_VERSION));
		LOG("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
	}

	//TEST

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;			//Enable docking
	ImGui::StyleColorsDark(); 

	ImGui_ImplOpenGL3_Init();

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'misc/fonts/README.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal 

	io->Fonts->AddFontDefault();
	io->Fonts->AddFontFromFileTTF("misc/fonts/Roboto-Medium.ttf", 13);
	io->Fonts->AddFontFromFileTTF("misc/fonts/GOTHIC.TTF", 14);

	io->FontDefault = ImGui::GetIO().Fonts->Fonts[2];
	return true;
}

// Update all guis
update_status ModuleGui::PreUpdate(float dt)
{
	BROFILER_CATEGORY("GuiPreUpdate", Profiler::Color::Orange)

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return UPDATE_CONTINUE;
}

// Called after all Updates
update_status ModuleGui::PostUpdate(float dt)
{
	BROFILER_CATEGORY("GuiPostUpdate", Profiler::Color::Yellow)

	bool opt_fullscreen = true;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	/*if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)*/
	dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
	window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &p_open, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	ImGui::End();
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleGui::CleanUp()
{
	LOG("Freeing Gui");
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}

void ModuleGui::DrawImGui(float dt) {

	// Menu bar
	/*glLineWidth(100.0f);
	glBegin(GL_POLYGON);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(100.f, 10.f, 220.f);
	glEnd();

	glClearColor(255, 0, 0, 0);*/

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

		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::Checkbox("Demo Window", &show_demo_window);
			ImGui::Checkbox("Settings", &show_settings);
			ImGui::Checkbox("Console", &show_console);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("About"))
		{
			ImGui::Text("Percularity v0.1");
			ImGui::Text("3D engine developed for student purposes");
			ImGui::Text("By Joan Marin & Daniel Lorenzo");
			if (ImGui::Button("Go to our GitHub"))
				ShellExecuteA(NULL, "open", "https://github.com/DLorenzoLaguno17/PercularityEngine", NULL, NULL, SW_SHOWNORMAL);
			ImGui::NewLine();

			ImGui::Separator();
			ImGui::Text("3rd party libraries used:");
			ImGui::BulletText("SDL 2.0.6");
			ImGui::BulletText("STL");
			ImGui::BulletText("Dear ImGui 1.72b");
			ImGui::BulletText("MathGeoLib 1.5");
			ImGui::BulletText("Open GL 4.5");
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

	// 1. Show the big demo window 
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// Show settings window
	if (show_settings) {
		settings.Update(dt, App);
	}

	if (show_console) {
		ImGui::Begin("Console");

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN) LOG("Esta wea funciona");

		for (int i = 0; i < log_list.size(); ++i) {

			ImGui::SetScrollHereY(1.0f);
			ImGui::Text(log_list[i].c_str());
		}
		
		ImGui::End();
	}

	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}