#ifndef __ModuleGui_H__
#define __ModuleGui_H__

#include "Module.h"
#include <string>
#include <vector>
#include "SettingsWindow.h"

#include "imgui.h"

#include "SDL/include/SDL_rect.h"
#include "SDL/include/SDL_video.h"

#define CURSOR_WIDTH 2
#define MAX_LOGS 30

struct SDL_Texture;
struct _TTF_Font;
struct SDL_Rect;
struct SDL_Color; 

enum UIELEMENT_TYPES
{
	BUTTON,
	CHECKBOX,
	INPUTTEXT,
	LABEL,
	IMAGE,
	BOX
};

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

	void DrawImGui(float dt);

public:
	bool show_demo_window = true;
	bool show_settings = true;
	bool show_console = true;
	bool show_scene = true;
	bool show_elements = true;

	ImGuiIO* io = nullptr;

private:
	SettingsWindow settings;
	bool p_open = true;
};

#endif // __ModuleGui_H__