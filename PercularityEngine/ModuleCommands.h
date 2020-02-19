#ifndef _MODULE_COMMANDS_H__
#define _MODULE_COMMANDS_H__

#include "Module.h"

class GameObject;

enum class ACTION_TYPE 
{
	TRANSFORM_GAMEOBJECT,
	MODIFY_GAMEOBJECT,
	LOAD_SCENE,
	NO_ACTION
};

class Action 
{
public:
	Action(ACTION_TYPE type = ACTION_TYPE::NO_ACTION) : type(type) {}
	virtual ~Action() {}

	// Action methods
	virtual void UndoAction() {}
	virtual void RedoAction() {}

	// To release the memory
	virtual bool CleanUp() { return true; }

public:
	ACTION_TYPE type = ACTION_TYPE::NO_ACTION;
};

class TransformAction : public Action
{
public:
	TransformAction(ACTION_TYPE type);
	~TransformAction() {}

	bool CleanUp();
	//void ExecuteAction() {}

public:

};

class ModifyAction : public Action
{
public:
	ModifyAction(ACTION_TYPE type);
	~ModifyAction() {}

	bool CleanUp();
	//void ExecuteAction() {}

public:
	GameObject* modified_go = nullptr;
};

// ------------------------------------------------------------
class ModuleCommands : public Module
{
public:

	ModuleCommands(Application* app, bool start_enabled = true);
	~ModuleCommands() {}

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	// Action methods
	Action* StoreAction(ACTION_TYPE type);
	void Undo();
	void Redo();

public:
	std::vector<Action*> userActions;

private:
	uint actionCursor = 0;
};
#endif