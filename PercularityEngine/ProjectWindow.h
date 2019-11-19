#ifndef __ProjectWindow_H__
#define __ProjectWindow_H__

#include "UIElement.h"

// ---------------------------------------------------
class ProjectWindow : public UIElement
{
public:
	ProjectWindow(char* name, bool active = true);

	// Destructor
	virtual ~ProjectWindow() {};

	// Called every frame
	void Update();

};

#endif // __SceneWindow_H__