#ifndef __ModuleGui_H__
#define __ModuleGui_H__

#include "Module.h"
#include "Globals.h"
#include <list>
#include <string>
#include "imgui.h"

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

	// Called every frame
	update_status Update(float dt);

	// Called after all Updates
	update_status PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

public:
	//our state
	bool show_demo_window = true;
	bool show_settings = true;

	bool fullscreen = false;
	bool resizable = false;
	bool borderless = false;
	bool fulldesktop = false;
	bool vsync = true;

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

private:
	SDL_GLContext gl_context; 
	ImGuiIO* io = nullptr;
	uint cnt = 0;
};

#endif // __ModuleGui_H__