#ifndef __ModuleGui_H__
#define __ModuleGui_H__

#include "Module.h"
#include <list>
#include <string>
#include "SDL/include/SDL_rect.h"

#define CURSOR_WIDTH 2

struct SDL_Texture;
struct _TTF_Font;
struct SDL_Rect;
struct SDL_Color; 
struct ImDrawData;

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
	bool PreUpdate();

	// Called every frame
	bool Update();

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void RenderFunction(ImDrawData* draw_data);

public:

private:
};

#endif // __ModuleGui_H__