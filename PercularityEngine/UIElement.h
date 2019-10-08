#ifndef __UIElement_H__
#define __UIElement_H__

// ---------------------------------------------------
class UIElement
{
public:

	UIElement(char* name, bool active = true) : name(name), active(active) {}

	// Destructor
	virtual ~UIElement() {}

	// Called every frame
	virtual void Update() {}

public:
	char* name;
	bool active;
};

#endif // __SettingsWindow_H__