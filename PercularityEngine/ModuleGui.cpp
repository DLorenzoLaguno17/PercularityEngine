#include "Application.h"
#include "ModuleInput.h"
#include "ModuleGui.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "GameObject.h"

#include "DebugWindow.h"
#include "OpenGL.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"

#include "Brofiler/Lib/Brofiler.h"
#include "mmgr/mmgr.h"

ModuleGui::ModuleGui(Application* app, bool start_enabled) : Module(app, start_enabled) {}

// Destructor
ModuleGui::~ModuleGui() {}

// Called before render is available
bool ModuleGui::Init()
{
	LOG("Loading GUI atlas");
	bool ret = true;

	IMGUI_CHECKVERSION();

	// Setting context
	SDL_GL_MakeCurrent(App->window->window, App->renderer3D->context);
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);

	// Setting GuiIO
	io = &ImGui::GetIO(); (void)io;
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;  //Enable docking

	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init();

	io->Fonts->AddFontDefault();
	io->Fonts->AddFontFromFileTTF("Fonts/Roboto-Medium.ttf", 13);
	io->Fonts->AddFontFromFileTTF("Fonts/Karla-Regular.ttf", 16);
	io->Fonts->AddFontFromFileTTF("Fonts/GOTHIC.ttf", 16);

	io->FontDefault = ImGui::GetIO().Fonts->Fonts[2];

	return ret;
}

// Called before the first frame
bool ModuleGui::Start()
{
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

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background	
	dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
	window_flags |= ImGuiWindowFlags_NoBackground;

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

	RELEASE(project);
	RELEASE(project);
	RELEASE(hierarchy);
	RELEASE(inspector);
	RELEASE(console);
	RELEASE(scene_window);
	RELEASE(settings);

	return true;
}

void ModuleGui::DrawImGui(float dt) {
	main_menu_bar->Update(ui_elements_list);

	// 1. Show the big demo window
	//if (show_demo_window)
		//ImGui::ShowDemoWindow(&show_demo_window);

	// Show windows if they are active
	for(int i = 0; i < ui_elements_list.size(); ++i)
		if (ui_elements_list[i]->active) ui_elements_list[i]->Update();	

	// Rendering
	ImGui::Render();
	//glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ModuleGui::Load(const json &config)
{
	settings = new ConfigWindow("Configuration", true);
	scene_window = new SceneWindow("Scene", true);
	console = new ConsoleWindow("Console", true);
	inspector = new InspectorWindow("Inspector", true);
	hierarchy = new HierarchyWindow("Hierarchy", true);
	project = new ProjectWindow("Project", true);
	debugWindow = new DebugWindow("Debug", true);
	ui_elements_list.push_back(settings);
	ui_elements_list.push_back(scene_window);
	ui_elements_list.push_back(console);
	ui_elements_list.push_back(inspector);
	ui_elements_list.push_back(hierarchy);
	ui_elements_list.push_back(project);
	ui_elements_list.push_back(debugWindow);

	for (int i = 0; i < ui_elements_list.size(); ++i)
		ui_elements_list[i]->active = config["User Interface"][ui_elements_list[i]->name];
}

void ModuleGui::Save(json &config)
{
	for (int i = 0; i < ui_elements_list.size(); ++i)
		config["User Interface"][ui_elements_list[i]->name] = ui_elements_list[i]->active;
}