#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

	//Save & Load
	void Load(const nlohmann::json  &config);
	void Save(nlohmann::json &config);

public:
	void SetTitle(const char* title);
	void SetFullscreen(bool goingFullscreen);
	void SetBorderless(bool goingBorderless);
	void SetResizable(bool goingResizable);
	void SetFulldesktop(bool goingFulldesktop);

	uint GetWindowWidth() const { return winWidth; }
	uint GetWindowHeight() const { return winHeight; }
	uint GetWindowSize() const { return winSize; }

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;
	bool winResizable;
	bool winBorderless;
	bool winFullscreen;
	bool winFullscreenDesktop;

private:
	//We set values here in order to avoid issues with release
	//but the right ones will be set on the Load() phase
	std::string winTitle;

	//Window size
	uint winWidth = 0;
	uint winHeight = 0;
	uint winSize = 1;

};				 

#endif // __ModuleWindow_H__