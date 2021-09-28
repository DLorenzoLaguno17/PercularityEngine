#ifndef __ModuleUndo_H__
#define __ModuleUndo_H__

#include "Module.h"

class Action 
{
public:
	virtual void Undo() {};
	virtual void Redo() {};
	virtual void CleanUp() {};
};

// ---------------------------------------------------
class ModuleUndo : public Module
{
public:
	ModuleUndo(Application* app, bool start_enabled = true) : Module(app, start_enabled) {}
	~ModuleUndo() {}

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	// Ctrl+Z and Ctrl+Y methods
	void Undo();
	void Redo();

	void StoreNewAction(Action* newAction);

private:
	std::vector<Action*> actions;
	std::vector<Action*> temporalActions;
	uint cursor = 0;
};

#endif // __ModuleUndo_H__