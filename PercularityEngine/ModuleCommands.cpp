#include "Application.h"
#include "ModuleCommands.h"

ModuleCommands::ModuleCommands(Application* app, bool start_enabled) :
	Module(app, start_enabled) {}

bool ModuleCommands::Init() 
{
	return true;
}

bool ModuleCommands::Start() 
{
	return true;
}

update_status ModuleCommands::PreUpdate(float dt) 
{
	return UPDATE_CONTINUE;
}

update_status ModuleCommands::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleCommands::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleCommands::CleanUp() 
{
	for (int i = 0; i < userActions.size(); ++i) {
		userActions[i]->CleanUp();
	}

	userActions.clear();
	return true;
}

// Action methods
Action* ModuleCommands::StoreAction(ACTION_TYPE type) 
{
	std::vector<Action*>::iterator act_it = std::find(userActions.begin(), userActions.end(), userActions[actionCursor]);
	userActions.erase(act_it, userActions.end());
	Action* action = nullptr;

	switch (type) 
	{
	case ACTION_TYPE::TRANSFORM_GAMEOBJECT:		
		action = new TransformAction(ACTION_TYPE::TRANSFORM_GAMEOBJECT);
		break;

	case ACTION_TYPE::MODIFY_GAMEOBJECT:
		action = new ModifyAction(ACTION_TYPE::MODIFY_GAMEOBJECT);
		break;

	/*case ACTION_TYPE::LOAD_SCENE:
		action = new TransformAction(ACTION_TYPE::LOAD_SCENE);
		break;*/
	}

	userActions.push_back(action);
	actionCursor++;
	return action;
}

void ModuleCommands::Undo() 
{
	userActions[actionCursor]->UndoAction();
	actionCursor--;
}

void ModuleCommands::Redo() {
	userActions[actionCursor]->RedoAction();
	actionCursor++;
}

// -----------------------------------------------------------
// DIFFERENT ACTIONS
// -----------------------------------------------------------

// Transform a GameObject
TransformAction::TransformAction(ACTION_TYPE type) : Action(type) {}

bool TransformAction::CleanUp() { return true; }

// Create or delete a GameObject
ModifyAction::ModifyAction(ACTION_TYPE type) : Action(type) {}	

bool ModifyAction::CleanUp() 
{ 
	RELEASE(modified_go);
	return true;
}
	