#ifndef __HierarchyWindow_H__
#define __HierarchyWindow_H__

#include "UIElement.h"

// ---------------------------------------------------
class HierarchyWindow : public UIElement
{
public:
	HierarchyWindow(char* name, bool active = true);

	// Destructor
	virtual ~HierarchyWindow() {};

	// Called every frame
	void Update();
};

#endif // __HierarchyWindow_H__