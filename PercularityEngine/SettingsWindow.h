#ifndef __SettingsWindow_H__
#define __SettingsWindow_H__

#include <vector>
#include "imgui.h"
#include "Timer.h"

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
	bool fulldesktop = true;
	bool vsync = true;

	bool cullFace = true;
	bool lighting = true;
	bool depthTest = true;
	bool colorMaterial = true;
	bool texture2D = true;

	bool showButtonInfo = false;
	bool showButtonInfo2 = false;
	int mouseButton2;
	int mouseButton;
	float secs;

private:
	std::vector<float> fps;
	std::vector<float> ms;

	bool timerStarted = false;
	Uint32 lastTime = 0;
	Timer dblcTimer; 
};

#endif // __SettingsWindow_H__