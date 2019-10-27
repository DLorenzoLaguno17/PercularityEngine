#ifndef __ModuleGui_H__
#define __ModuleGui_H__

#include "Module.h"
#include <string>
#include <vector>
#include "ConfigWindow.h"
#include "SceneWindow.h"
#include "ConsoleWindow.h"
#include "InspectorWindow.h"
#include "HierarchyWindow.h"
#include "MainMenuBar.h"

#include "imgui.h"

#include "SDL/include/SDL_rect.h"
#include "SDL/include/SDL_video.h"

#define CURSOR_WIDTH 2

struct SDL_Texture;
struct _TTF_Font;
struct SDL_Rect;
struct SDL_Color; 

// ---------------------------------------------------
class ModuleGui : public Module
{
public:
	ModuleGui(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleGui();

	// Called when before render is available
	bool Awake();

	// Call before first frame
	bool Start();

	// Called before all Updates
	update_status PreUpdate(float dt);

	// Called after all Updates
	update_status PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	//Save & Load
	void Load(const nlohmann::json  &config);
	void Save(nlohmann::json &config);

	void DrawImGui(float dt);

public:
	//bool show_demo_window = true;

	ImGuiIO* io = nullptr;
	SceneWindow* scene_window = nullptr;

private:
	ConfigWindow* settings = nullptr;
	MainMenuBar* main_menu_bar = nullptr;
	ConsoleWindow* console = nullptr;
	InspectorWindow* inspector = nullptr;
	HierarchyWindow* hierarchy = nullptr;

	std::vector<UIElement*> ui_elements_list;

	bool p_open = true;
};

#endif // __ModuleGui_H__
