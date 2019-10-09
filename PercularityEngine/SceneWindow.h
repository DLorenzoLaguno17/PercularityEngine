#ifndef __SceneWindow_H__
#define __SceneWindow_H__

#include "UIElement.h"
#include "Globals.h"

// ---------------------------------------------------
class SceneWindow : public UIElement
{
public:
	SceneWindow(char* name, bool active = true);

	// Destructor
	virtual ~SceneWindow() {};

	// Called every frame
	void Update();

private:
	ImVec2 last_windowSize = { 0,0 };
};

#endif // __SceneWindow_H__