#ifndef __ConsoleWindow_H__
#define __ConsoleWindow_H__

#include "UIElement.h"

// ---------------------------------------------------
class ConsoleWindow : public UIElement
{
public:
	ConsoleWindow(char* name, bool active = true);

	// Destructor
	virtual ~ConsoleWindow() {};

	// Called every frame
	void Update();

};

#endif // __SceneWindow_H__