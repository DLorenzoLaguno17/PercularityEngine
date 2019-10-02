#ifndef __SettingsWindow_H__
#define __SettingsWindow_H__

#include <vector>
#include "imgui.h"

class Application;

// ---------------------------------------------------
class SettingsWindow
{
public:
	SettingsWindow() {};

	// Destructor
	virtual ~SettingsWindow() {};


	// Called every frame
	void Update(float dt, Application* App);


	// Called before quitting
	bool CleanUp();

public:
	bool fullscreen = false;
	bool resizable = false;
	bool borderless = false;
	bool fulldesktop = false;
	bool vsync = true;

	bool cullFace = true;
	bool lighting = true;
	bool depthTest = true;
	bool colorMaterial = true;
	bool texture2D = true;

private:
	std::vector<float> fps;
	std::vector<float> ms;

};

#endif // __ModuleGui_H__