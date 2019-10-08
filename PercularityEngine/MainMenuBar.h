#ifndef __MainMenuBar_H__
#define __MainMenuBar_H__

#include <vector>
#include "imgui.h"

class Application;

// ---------------------------------------------------
class MainMenuBar
{
public:
	MainMenuBar() {};

	// Destructor
	virtual ~MainMenuBar() {};

	// Called every frame
	void Update(Application* App);

	// Called before quitting
	bool CleanUp();

};

#endif // __MainManuBar_H__