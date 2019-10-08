#ifndef __MainMenuBar_H__
#define __MainMenuBar_H__

#include <vector>
class UIElement;

// ---------------------------------------------------
class MainMenuBar
{
public:
	MainMenuBar() {};

	// Destructor
	virtual ~MainMenuBar() {};

	// Called every frame
	void Update(std::vector<UIElement*> list);
};

#endif // __MainManuBar_H__