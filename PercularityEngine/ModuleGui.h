#ifndef __ModuleGui_H__
#define __ModuleGui_H__

#include "Module.h"
#include "Globals.h"
#include <list>
#include <string>
#include "SettingsWindow.h"

#include "SDL/include/SDL_rect.h"
#include "SDL/include/SDL_video.h"

#define CURSOR_WIDTH 2

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
	//our state
	bool show_demo_window = true;
	bool show_settings = true;
	ImGuiIO* io = nullptr;

private:
	SDL_GLContext gl_context; 
	SettingsWindow settings;
};

#endif // __ModuleGui_H__