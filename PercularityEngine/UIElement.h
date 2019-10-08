#ifndef __UIElement_H__
#define __UIElement_H__

class Application;

class UIElement
{
public:
	// Called before all Updates
	virtual void Update(Application* App) {};

public:
	bool active = true;
	char* name;
};

#endif // __UIElement_H__