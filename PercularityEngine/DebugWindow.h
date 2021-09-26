#ifndef __DEBUG_WINDOW_H__
#define __DEBUG_WINDOW_H__

#include "UIElement.h"

class DebugWindow : public UIElement
{
public:

	DebugWindow(char* name, bool active = true);

	// Destructor
	virtual ~DebugWindow() {};

	// Called every frame
	void Update();

public:
	bool activateConstraint = false;
	bool debugPhysics = false;
};

#endif