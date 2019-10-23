#ifndef __InspectorWindow_H__
#define __InspectorWindow_H__

#include "UIElement.h"

// ---------------------------------------------------
class InspectorWindow : public UIElement
{
public:
	InspectorWindow(char* name, bool active = true);

	// Destructor
	virtual ~InspectorWindow() {};

	// Called every frame
	void Update();
};

#endif // __InspectorWindow_H__