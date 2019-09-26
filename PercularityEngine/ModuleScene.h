#ifndef _MODULE_SCENE_H__
#define _MODULE_SCENE_H__

#include "Module.h"

class ModuleScene :public Module
{
public:

	ModuleScene(Application* app, bool start_enabled);
	~ModuleScene();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

};

#endif