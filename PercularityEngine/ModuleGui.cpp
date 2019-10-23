#include "Application.h"
#include "ModuleInput.h"
#include "ModuleGui.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"

#include "OpenGL.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "Brofiler/Lib/Brofiler.h"

#include "mmgr/mmgr.h"

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

		LOG("Vendor: %s", glGetString(GL_VENDOR));
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
	io->Fonts->AddFontFromFileTTF("Fonts/Roboto-Medium.ttf", 13);
	io->Fonts->AddFontFromFileTTF("Fonts/GOTHIC.TTF", 16);

	io->FontDefault = ImGui::GetIO().Fonts->Fonts[2];

	settings = new ConfigWindow("Configuration", true); 
	scene_window = new SceneWindow("Scene", true);
	console = new ConsoleWindow("Console", true);
	ui_elements_list.push_back(settings);
	ui_elements_list.push_back(scene_window);
	ui_elements_list.push_back(console);

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

	delete console; console = nullptr;
	delete scene_window; scene_window = nullptr;
	delete settings; settings = nullptr;

	return true;
}

void ModuleGui::DrawImGui(float dt) {


	main_menu_bar->Update(ui_elements_list);

	// 1. Show the big demo window
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// Show windows if they are active
	for(int i = 0; i < ui_elements_list.size(); ++i)
		if (ui_elements_list[i]->active) ui_elements_list[i]->Update();	

	if (show_hierarchy) {
		ImGui::Begin("Hierarchy", &show_hierarchy);

		ImGui::End();
	}

	if (show_project) {
		ImGui::Begin("Project", &show_project);

		ImGui::End();
	}

	if (show_inspector) {
		ImGui::Begin("Inspector", &show_inspector);

		ImGui::End();
	}

	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ModuleGui::Load(const json &config)
{
	for (int i = 0; i < ui_elements_list.size(); ++i)
		ui_elements_list[i]->active = config["User Interface"][ui_elements_list[i]->name];
}

void ModuleGui::Save(json &config)
{
	for (int i = 0; i < ui_elements_list.size(); ++i)
		config["User Interface"][ui_elements_list[i]->name] = ui_elements_list[i]->active;
}