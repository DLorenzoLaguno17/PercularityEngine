#ifndef __ConfigWindow_H__
#define __ConfigWindow_H__

#include "Timer.h"
#include "UIElement.h"

// ---------------------------------------------------
class ConfigWindow : public UIElement
{
public:
	ConfigWindow(char* name, bool active = true);

	// Destructor
	virtual ~ConfigWindow() {};

	// Called every frame
	void Update();

public:
	bool fullscreen = false;
	bool resizable = false;
	bool borderless = false;
	bool fulldesktop = true;
	bool vsync = true;

	bool cullFace = true;
	bool normals = false;
	bool lighting = true;
	bool depthTest = true;
	bool colorMaterial = true;
	bool texture2D = true;
	bool wireframe = false;

	bool showButtonInfo = false;
	bool showButtonInfo2 = false;
	bool showKeyInfo = false;
	bool showKeyInfo2 = false;
	int mouseButton2;
	int mouseButton;
	int keyButton; 
	ImWchar c;
	float m_secs;
	float k_secs;

private:
	std::vector<float> fps;
	std::vector<float> ms;

	Uint32 lastTime = 0;
	Timer dblcTimer; 
};

#endif // __ConfigWindow_H__