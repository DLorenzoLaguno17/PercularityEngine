#ifndef __SceneWindow_H__
#define __SceneWindow_H__

#include "UIElement.h"

// ---------------------------------------------------
class SceneWindow : public UIElement
{
public:
	SceneWindow(char* name, bool active = true);

	// Destructor
	virtual ~SceneWindow() {};

	// Called every frame
	void Update();

};

#endif // __SceneWindow_H__