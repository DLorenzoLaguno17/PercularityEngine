#ifndef __ModuleUndo_H__
#define __ModuleUndo_H__

#include "Module.h"

enum class ACTION_TYPE
{
	NO_ACTION,
	ADD_GAMEOBJECT,
	TRANSLATE_GAMEOBJECT,
	ROTATE_GAMEOBJECT,
	SCALE_GAMEOBJECT,
};

class Action 
{
public:
	Action(ACTION_TYPE type = ACTION_TYPE::NO_ACTION) : type(type) {};

	virtual void Undo() {};
	virtual void Redo() {};

	ACTION_TYPE type;
};

// ---------------------------------------------------
class ModuleUndo : public Module
{
public:
	ModuleUndo(Application* app, bool start_enabled = true) : Module(app, start_enabled) {}

	// Destructor
	virtual ~ModuleUndo() {}

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Undo();
	void Redo();

	void StoreNewAction(Action* newAction);

private:
	std::vector<Action*> actions;
	uint cursor = 0;
};

#endif // __ModuleUndo_H__