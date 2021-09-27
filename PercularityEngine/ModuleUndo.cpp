#include "Application.h"
#include "ModuleUndo.h"
#include "ModuleInput.h"
#include "Time.h"

#include "Brofiler/Lib/Brofiler.h"
#include "mmgr/mmgr.h"

bool ModuleUndo::Start()
{
	return true;
}

update_status ModuleUndo::Update(float dt)
{
	BROFILER_CATEGORY("UndoUpdate", Profiler::Color::LightSeaGreen);

	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN) Undo();
		if (App->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN) Redo();
	}

	return UPDATE_CONTINUE;
}

bool ModuleUndo::CleanUp()
{
	for (uint i = 0; i < actions.size(); ++i)
	{
		actions[i]->CleanUp();
		RELEASE(actions[i]);
	}

	actions.clear();
	cursor = 0;
	return true;
}

void ModuleUndo::Undo()
{
	if (cursor > 0)
	{
		actions[cursor - 1]->Undo();
		cursor--;
	}
};

void ModuleUndo::Redo() 
{
	if (cursor < actions.size())
	{
		cursor++;
		actions[cursor - 1]->Redo();
	}
};

void ModuleUndo::StoreNewAction(Action* newAction)
{
	if (Time::running) 
	{
		//temporalActions.push_back(newAction);
	}
	else
	{
		if (cursor != actions.size())
		{
			for (uint i = actions.size(); i > cursor; --i)
			{
				actions.back()->CleanUp();
				RELEASE(actions.back());
				actions.pop_back();
			}
		}

		actions.push_back(newAction);
		cursor = actions.size();
	}
}