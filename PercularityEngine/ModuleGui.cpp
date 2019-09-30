#include "Application.h"
#include "ModuleInput.h"
#include "ModuleGui.h"
#include "ModuleWindow.h"

#include "SDL/include/SDL.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <vector>
#include "gl3w.h"

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
	ImGui::CreateContext();
	(void)io;

	// Setting context
	gl_context = SDL_GL_CreateContext(App->window->window);
	SDL_GL_MakeCurrent(App->window->window, gl_context);

	//TEST
	io = &ImGui::GetIO(); (void)io;
	//TEST

	//TEST
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
	bool err = gl3wInit() != 0;
#endif
	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}

	//TEST

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark(); 
	
	//const char* glsl_version = "#version 130";

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);

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

	return true;
}

// Update all guis
update_status ModuleGui::PreUpdate(float dt)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return  UPDATE_CONTINUE;
}

// Called every frame
update_status ModuleGui::Update(float dt)
{
	// Menu bar
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
				return UPDATE_STOP;

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::MenuItem("Test...");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::Checkbox("Demo Window", &show_demo_window);
			ImGui::Checkbox("Settings", &show_settings);	
			ImGui::EndMenu();
		}		

		if (ImGui::BeginMenu("About"))
		{
			ImGui::Text("Percularity v0.1");
			ImGui::Text("3D engine developed for student purposes");
			ImGui::Text("By Joan Marin & Daniel Lorenzo"); 
			if (ImGui::MenuItem("Go to our GitHub"))
				ShellExecuteA(NULL, "open", "https://github.com/DLorenzoLaguno17/PercularityEngine", NULL, NULL, SW_SHOWNORMAL);
			ImGui::NewLine();

			ImGui::Separator();
			ImGui::Text("3rd party libraries used:");
			ImGui::BulletText("SDL 2.0.6");
			ImGui::BulletText("Dear ImGui");
			ImGui::BulletText("MathGeoLib");
			ImGui::BulletText("Open GL");
			ImGui::NewLine();

			ImGui::Separator();
			ImGui::Text("License");
			ImGui::NewLine();
			ImGui::Text("MIT License");
			ImGui::NewLine();
			ImGui::Text("Copyright(c) 2019 Joan Marin & Dani Lorenzo");
			ImGui::NewLine();
			ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy");
			ImGui::Text("of this software and associated documentation files (the ""Software""), to deal");
			ImGui::Text("in the Software without restriction, including without limitation the rights");
			ImGui::Text("to use, copy, modify, merge, publish, distribute, sublicense, and/or sell");
			ImGui::Text("copies of the Software, and to permit persons to whom the Software is");
			ImGui::Text("furnished to do so, subject to the following conditions:");
			ImGui::NewLine();
			ImGui::Text("The above copyright notice and this permission notice shall be included in all");
			ImGui::Text("copies or substantial portions of the Software.");
			ImGui::NewLine();
			ImGui::Text("THE SOFTWARE IS PROVIDED ""AS IS"", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR");
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

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);
	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	if (show_settings){
		static float f = 0.0f;
		ImGui::Begin("Settings");         // Creates the settings window		
		
		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)		
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("Background color", (float*)&clear_color); // Edit 3 floats representing a color

		ImGui::NewLine();
		if (ImGui::Checkbox("Fullscreen", &fullscreen))
			App->window->SetFullscreen(fullscreen);
		ImGui::SameLine();
		if (ImGui::Checkbox("Resizable", &resizable))
			App->window->SetResizable(resizable);
		if (ImGui::Checkbox("Borderless", &borderless))
			App->window->SetBorderless(borderless);
		ImGui::SameLine();
		if (ImGui::Checkbox("Full desktop", &fulldesktop))
			App->window->SetFulldesktop(fulldesktop);

		ImGui::NewLine();
		ImGui::ShowFontSelector("Select font");

		ImGui::Separator();
		if (ImGui::Checkbox("V. Sync", &vsync)) {}
		ImGui::NewLine();
		static float arr[] = { 23.6f, 78.1f, 34.0f, 42.5f, 76.92f, 90.1f, 21.2f };

		/*char title1[25];
		float fps[100];
		fps[cnt] = io->Framerate;
		sprintf_s(title1, 25, "Framerate %.1f", fps[cnt-1]);
		ImGui::PlotHistogram("##framerate", arr, cnt, 0, title1, 0.0f, 100.0f, ImVec2(310, 100));
		char title2[25];
		sprintf_s(title2, 25, "Milliseconds %.3f", 1000.0f / io->Framerate);
		ImGui::PlotHistogram("##milliseconds", arr, 20, 0, title2, 0.0f, 40.0f, ImVec2(310, 100));
		cnt++;*/

		ImGui::End();
	}

	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	//SDL_GL_SwapWindow(App->window->window);	

	return  UPDATE_CONTINUE;
}

// Called after all Updates
update_status ModuleGui::PostUpdate(float dt)
{

	return  UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleGui::CleanUp()
{
	LOG("Freeing Gui");
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	SDL_GL_DeleteContext(gl_context);

	return true;
}